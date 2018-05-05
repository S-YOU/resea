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
        self.types = []
        self.calls = []

    def exitService(self, ctx):
        self.name = str(ctx.getChild(2))
        self.id = int(str(ctx.getChild(5)))

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
    return {
        "name": listener.name,
        "id": listener.id,
        "types": listener.types,
        "calls": listener.calls
    }


def generate_stub(service):
    service_name = service["name"].replace("-", "_")
    server_includes = ""
    server_mainloop = ""
    server_handlers = ""
    server_includes += f"#include <resea/{service_name}.h>"
    client_stub = ""
    types = {}

    # Type aliases.
    for type_ in service["types"]:
        alias_of = type_["alias_of"] + "_t"
        client_stub += f"typedef {alias_of} {type_['new_name']};\n"
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
            "umax": INLINE_PAYLOAD,
            "error": INLINE_PAYLOAD,
            "i8": INLINE_PAYLOAD,
            "i16": INLINE_PAYLOAD,
            "i32": INLINE_PAYLOAD,
            "i64": INLINE_PAYLOAD,
            "u8": INLINE_PAYLOAD,
            "u16": INLINE_PAYLOAD,
            "u32": INLINE_PAYLOAD,
            "u64": INLINE_PAYLOAD
        }[name]

    # RPCs.
    client_stub += "\n"
    msg_id = 1
    for call in service["calls"]:
        call_name = call["name"]
        header = "0"
        reply_header = "0"
        args = ""
        params = ""
        server_params = ""
        for i in range(0, 4):
            try:
                name = call["args"][i]["name"]
                type_ = call["args"][i]["type"]
            except IndexError:
                params += ", 0"
            else:
                type_id = get_type_id_by_name(type_)
                header += f" | ({type_id} << {i * 3}ULL)"
                args += f", {type_}_t {name}"
                params += f", (payload_t) {name}"
                server_params += f", ({type_}_t) a{i}"

        for i in range(0, 4):
            try:
                name = call["rets"][i]["name"]
                type_ = call["rets"][i]["type"]
            except IndexError:
                params += ", &__unused"
            else:
                type_id = get_type_id_by_name(type_)
                reply_header += f" | ({type_id} << {i * 3}ULL)"
                args += f", {type_}_t *{name}"
                params += f", (payload_t *) {name}"
                server_params += f", ({type_}_t *) &r{i}"

        msg_name = f"{service_name.upper()}_{call_name.upper()}_MSG"
        reply_msg_name = f"{service_name.upper()}_{call_name.upper()}_REPLY_MSG"
        header_name = f"{service_name.upper()}_{call_name.upper()}_HEADER"
        reply_header_name = f"{service_name.upper()}_{call_name.upper()}_REPLY_HEADER"
        client_stub += f"""\
#define {msg_name}       ({service_name.upper()}_SERVICE | {msg_id}ULL)
#define {reply_msg_name} ({service_name.upper()}_SERVICE | {msg_id + 1}ULL)
#define {header_name} (({msg_name} << 32ULL) | ({header}))
#define {reply_header_name} (({reply_msg_name} << 32ULL) | ({reply_header}))
static inline header_t call_{service_name}_{call_name}(channel_t __server{args}) {{
    payload_t __unused;

    return ipc_call(
        __server, {header_name}{params}
    );
}}
"""

        server_handlers += f"""\
static inline error_t handle_{service_name}_{call_name}(channel_t from{args}) {{
    /* TODO */
    return ERROR_NONE;
}}

"""

        server_mainloop += f"""\
            case {msg_name}:
                error_t error = handle_{service_name}_{call_name}(from{server_params});
                header = {reply_msg_name} | error;
                break;
"""
        msg_id += 2 # request & reply messages

    # Enclose by a include guard.
    client_stub = f"""\
#ifndef __RESEA_STUB_{service_name}_H__
#define __RESEA_STUB_{service_name}_H__

#define {service_name.upper()}_SERVICE ({service["id"]}U << 8U)

{client_stub}

#endif

"""

    server_scaffold = f"""\
#include <resea.h>
#include <resea/{service_name}.h>

{server_handlers}
void {service_name}_server_mainloop(void) {{
    channel_t from;
    payload_t a0, a1, a2, a3;
    payload_t r0, r1, r2, r3;
    header_t header = ipc_recv(server, &from, &a0, &a1, &a2, &a3);
    for (;;) {{
        switch (MSGTYPE(header)) {{
{server_mainloop}
            default:
                /* Unknown message. */
                break;
        }}

        ipc_replyrecv(server, &from, header, r0, r1, r2, r3, &a0, &a1, &a2, &a3);
    }}
}}


void main(void) {{
    {service_name}_server_mainloop();
}}
"""

    return service_name, client_stub, server_scaffold


def main(argv):
    argparser = argparse.ArgumentParser()
    argparser.add_argument('-o', required=True)
    argparser.add_argument('--scaffold', action="store_true")
    argparser.add_argument('idl_file')
    args = argparser.parse_args()

    service = parse_idl(args.idl_file)
    service_name, stub, server = generate_stub(service)
    with open(os.path.join(args.o, service_name + ".h"), 'w') as f:
        f.write(stub)

    if args.scaffold:
        with open(os.path.join(args.o, service_name + ".c"), 'w') as f:
            f.write(server)

if __name__ == '__main__':
    main(sys)
