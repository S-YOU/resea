#!/usr/bin/env python3
import argparse
import os
import sys
from antlr4 import *
from parser.idlLexer import idlLexer
from parser.idlParser import idlParser
from parser.idlListener import idlListener

class Listener(idlListener):
    def __init__(self):
        self.services = []

    def enterService(self, ctx):
        self.types = []
        self.calls = []

    def exitService(self, ctx):
        self.services.append({
            "name": str(ctx.getChild(1)),
            "types": self.types,
            "calls": self.calls
        })

    def exitCallDef(self, ctx):
        self.calls.append({
            "name": str(ctx.getChild(0)),
            "args": ctx.getChild(2).args,
            "rets": ctx.getChild(6).args
        })

        for i in range(0, ctx.getChildCount()):
            child = ctx.getChild(i)
            if isinstance(child, TerminalNode):
                break

    def exitArgList(self, ctx):
        ctx.args = []
        for i in range(0, ctx.getChildCount()):
            child = ctx.getChild(i)
            if child == ')':
                break

            if isinstance(child, TerminalNode):
                continue

            ctx.args.append({
                "name": child.name,
                "type": child.type
            })

    def exitArg(self, ctx):
        ctx.name = str(ctx.getChild(0))
        ctx.type = str(ctx.getChild(2))

    def exitTypeDef(self, ctx):
        self.types.append({
            "new_name": str(ctx.getChild(1)),
            "alias_of": str(ctx.getChild(3))
        })


def parse_idl(filepath):
    lexer = idlLexer(FileStream(filepath))
    stream = CommonTokenStream(lexer)
    parser = idlParser(stream)
    tree = parser.idl()
    walker = ParseTreeWalker()
    listener = Listener()
    walker.walk(listener, tree)
    return listener.services


def generate_stub(services):
    for service in services:
        service_name = service["name"]
        types = {}
        stub = ""

        # Type aliases.
        for type_ in service["types"]:
            alias_of = type_["alias_of"] + "_t"
            stub += f"typedef {alias_of} {type_['new_name']};\n"
            types[type_["new_name"]] = type_["alias_of"]

        def get_type_id_by_name(name):
            INLINE_PAYLOAD = 0
            OOL_PAYLOAD = 1
            CHANNEL_PAYLOAD = 2
            name = types.get(name, name)
            return {
                "channel": CHANNEL_PAYLOAD,
                "string": OOL_PAYLOAD,
                "usize": INLINE_PAYLOAD,
                "uintmax": INLINE_PAYLOAD,
                "i8": INLINE_PAYLOAD,
                "i16": INLINE_PAYLOAD,
                "i32": INLINE_PAYLOAD,
                "i64": INLINE_PAYLOAD,
                "u8": INLINE_PAYLOAD,
                "u16": INLINE_PAYLOAD,
                "u32": INLINE_PAYLOAD,
                "u64": INLINE_PAYLOAD
            }[name]

        # Call stubs.
        stub += "\n"
        msg_id = 0
        for call in service["calls"]:
            msg_id += 1
            header = "0"
            args = ""
            params = ""
            for i in range(0, 4):
                try:
                    name = call["args"][i]["name"]
                    type_ = call["args"][i]["type"]
                except IndexError:
                    params += ", 0"
                else:
                    type_id = get_type_id_by_name(type_)
                    header += f" | ({type_id} << 8 + ({i} * 3))"
                    args += f", {type_}_t {name}"
                    params += f", (payload_t) {name}"

            for i in range(0, 4):
                try:
                    name = call["rets"][i]["name"]
                    type_ = call["rets"][i]["type"]
                except IndexError:
                    params += ", &__unused"
                else:
                    type_id = get_type_id_by_name(type_)
                    header += f" | ({type_id} << 8 + ({i + 4} * 3))"
                    args += f", {type_}_t *{name}"
                    params += f", (payload_t *) {name}"

#            for i in range(0, 3):
#                name = call["rets"][i]["name"]
#                type_ = call["rets"][i]["type"]
#                args += f", {type_}_t *{name}"
#                params += f", (payload_t) {name}"

            msg_name = f"{service_name.upper()}_{call['name'].upper()}_MSG"
            header_name = f"{service_name.upper()}_{call['name'].upper()}_HEADER"
            stub += f"""\
#define {msg_name}    ({msg_id})
#define {header_name} (({msg_name} << 32) | ({header}))
static inline header_t call_{service["name"]}_{call["name"]}(channel_t __server{args}) {{
    payload_t __unused;

    return ipc_call(
        __server, {header_name}{params}
    );
}}
"""

        # Enclose by a include guard.
        return f"""\
#ifndef __RESEA_STUB_{service_name}_H__
#define __RESEA_STUB_{service_name}_H__

{stub}

#endif

"""


def main(argv):
    argparser = argparse.ArgumentParser()
    argparser.add_argument('idl')
    argparser.add_argument('--idl-dir', default=".")
    argparser.add_argument('-o')
    args = argparser.parse_args()

    services = parse_idl(os.path.join(args.idl_dir, args.idl + '.idl'))
    with open(args.o, 'w') as f:
        stub = generate_stub(services)
        f.write(stub)

if __name__ == '__main__':
    main(sys)
