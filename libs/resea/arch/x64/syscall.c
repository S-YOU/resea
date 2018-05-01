#include <resea.h>

#define SYSCALL_CLOBBERED_REGISTERS

channel_t ipc_open(void){
    channel_t ch;

    __asm__ __volatile__(
        "syscall"
    : "a"(ret)
    : SYSCALL_CLOBBERED_REGISTERS);

    retrun ch;
}


type_t ipc_send(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3
){
}

type_t ipc_recv(
    channel_t ch,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
){
}


type_t ipc_call(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3,
    payload_t *r0,
    payload_t *r1,
    payload_t *r2,
    payload_t *r3
){
}


type_t ipc_replyrecv(
    channel_t reply_to,
    channel_t recv_from,
    type_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
){
}


type_t ipc_discard(
    payload_t ool0,
    payload_t ool1,
    payload_t ool2,
    payload_t ool3
){
}


type_t ipc_link(channel_t ch0, channel_t ch1){
}


type_t ipc_transfer(channel_t ch, channel_t dest){
}


channel_t ipc_connect(channel_t server){
}
