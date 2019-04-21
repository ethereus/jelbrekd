/*
 * IDENTIFICATION:
 * stub generated Thu Mar  8 12:11:20 2018
 * with a MiG generated by bootstrap_cmds-96.20.2
 * OPTIONS:
 */

/* Module jailbreak_daemon */

//str8 from electra1131

#define    __MIG_check__Request__jailbreak_daemon_subsystem__ 1

#include "jelbrek_server.h"

#ifndef    mig_internal
#define    mig_internal    static __inline__
#endif    /* mig_internal */

#ifndef    mig_external
#define mig_external
#endif    /* mig_external */

#if    !defined(__MigTypeCheck) && defined(TypeCheck)
#define    __MigTypeCheck        TypeCheck    /* Legacy setting */
#endif    /* !defined(__MigTypeCheck) */

#if    !defined(__MigKernelSpecificCode) && defined(_MIG_KERNEL_SPECIFIC_CODE_)
#define    __MigKernelSpecificCode    _MIG_KERNEL_SPECIFIC_CODE_    /* Legacy setting */
#endif    /* !defined(__MigKernelSpecificCode) */

#ifndef    LimitCheck
#define    LimitCheck 0
#endif    /* LimitCheck */

#ifndef    min
#define    min(a,b)  ( ((a) < (b))? (a): (b) )
#endif    /* min */

#if !defined(_WALIGN_)
#define _WALIGN_(x) (((x) + 3) & ~3)
#endif /* !defined(_WALIGN_) */

#if !defined(_WALIGNSZ_)
#define _WALIGNSZ_(x) _WALIGN_(sizeof(x))
#endif /* !defined(_WALIGNSZ_) */

#ifndef    UseStaticTemplates
#define    UseStaticTemplates    0
#endif    /* UseStaticTemplates */

#ifndef    __DeclareRcvRpc
#define    __DeclareRcvRpc(_NUM_, _NAME_)
#endif    /* __DeclareRcvRpc */

#ifndef    __BeforeRcvRpc
#define    __BeforeRcvRpc(_NUM_, _NAME_)
#endif    /* __BeforeRcvRpc */

#ifndef    __AfterRcvRpc
#define    __AfterRcvRpc(_NUM_, _NAME_)
#endif    /* __AfterRcvRpc */

#ifndef    __DeclareRcvSimple
#define    __DeclareRcvSimple(_NUM_, _NAME_)
#endif    /* __DeclareRcvSimple */

#ifndef    __BeforeRcvSimple
#define    __BeforeRcvSimple(_NUM_, _NAME_)
#endif    /* __BeforeRcvSimple */

#ifndef    __AfterRcvSimple
#define    __AfterRcvSimple(_NUM_, _NAME_)
#endif    /* __AfterRcvSimple */

#define novalue void

#define msgh_request_port    msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)    MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port        msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)    MACH_MSGH_BITS_REMOTE(bits)

#define MIG_RETURN_ERROR(X, code)    {\
((mig_reply_error_t *)X)->RetCode = code;\
((mig_reply_error_t *)X)->NDR = NDR_record;\
return;\
}

/* Forward Declarations */


mig_internal novalue _Xcall
(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP);


#if ( __MigTypeCheck )
#if __MIG_check__Request__jailbreak_daemon_subsystem__
#if !defined(__MIG_check__Request__call_t__defined)
#define __MIG_check__Request__call_t__defined

mig_internal kern_return_t __MIG_check__Request__call_t(__attribute__((__unused__)) __Request__call_t *In0P)
{
    
    typedef __Request__call_t __Request;
#if    __MigTypeCheck
    if ((In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
        (In0P->Head.msgh_size != (mach_msg_size_t)sizeof(__Request)))
        return MIG_BAD_ARGUMENTS;
#endif    /* __MigTypeCheck */
    
    return MACH_MSG_SUCCESS;
}
#endif /* !defined(__MIG_check__Request__call_t__defined) */
#endif /* __MIG_check__Request__jailbreak_daemon_subsystem__ */
#endif /* ( __MigTypeCheck ) */


/* Routine call */
mig_internal novalue _Xcall
(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
    
#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
        NDR_record_t NDR;
        uint8_t command;
        char commandPad[3];
        uint32_t pid;
        mach_msg_trailer_t trailer;
    } Request __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif
    typedef __Request__call_t __Request;
    typedef __Reply__call_t Reply __attribute__((unused));
    
    /*
     * typedef struct {
     *     mach_msg_header_t Head;
     *     NDR_record_t NDR;
     *     kern_return_t RetCode;
     * } mig_reply_error_t;
     */
    
    Request *In0P = (Request *) InHeadP;
    Reply *OutP = (Reply *) OutHeadP;
#ifdef    __MIG_check__Request__call_t__defined
    kern_return_t check_result;
#endif    /* __MIG_check__Request__call_t__defined */
    
    __DeclareRcvRpc(500, "call")
    __BeforeRcvRpc(500, "call")
    
#if    defined(__MIG_check__Request__call_t__defined)
    check_result = __MIG_check__Request__call_t((__Request *)In0P);
    if (check_result != MACH_MSG_SUCCESS)
    { MIG_RETURN_ERROR(OutP, check_result); }
#endif    /* defined(__MIG_check__Request__call_t__defined) */
    
    OutP->RetCode = jbd_call(In0P->Head.msgh_request_port, In0P->command, In0P->pid);
    
    OutP->NDR = NDR_record;
    
    
    __AfterRcvRpc(500, "call")
}



/* Description of this subsystem, for use in direct RPC */
const struct jbd_jailbreak_daemon_subsystem jbd_jailbreak_daemon_subsystem = {
    jailbreak_daemon_server_routine,
    500,
    501,
    (mach_msg_size_t)sizeof(union __ReplyUnion__jbd_jailbreak_daemon_subsystem),
    (vm_address_t)0,
    {
        { (mig_impl_routine_t) 0,
            (mig_stub_routine_t) _Xcall, 3, 0, (routine_arg_descriptor_t)0, (mach_msg_size_t)sizeof(__Reply__call_t)},
    }
};

mig_external boolean_t jailbreak_daemon_server
(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
    /*
     * typedef struct {
     *     mach_msg_header_t Head;
     *     NDR_record_t NDR;
     *     kern_return_t RetCode;
     * } mig_reply_error_t;
     */
    
    register mig_routine_t routine;
    
    OutHeadP->msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InHeadP->msgh_bits), 0);
    OutHeadP->msgh_remote_port = InHeadP->msgh_reply_port;
    /* Minimal size: routine() will update it if different */
    OutHeadP->msgh_size = (mach_msg_size_t)sizeof(mig_reply_error_t);
    OutHeadP->msgh_local_port = MACH_PORT_NULL;
    OutHeadP->msgh_id = InHeadP->msgh_id + 100;
    OutHeadP->msgh_reserved = 0;
    
    if ((InHeadP->msgh_id > 500) || (InHeadP->msgh_id < 500) ||
        ((routine = jbd_jailbreak_daemon_subsystem.routine[InHeadP->msgh_id - 500].stub_routine) == 0)) {
        ((mig_reply_error_t *)OutHeadP)->NDR = NDR_record;
        ((mig_reply_error_t *)OutHeadP)->RetCode = MIG_BAD_ID;
        return FALSE;
    }
    (*routine) (InHeadP, OutHeadP);
    return TRUE;
}

mig_external mig_routine_t jailbreak_daemon_server_routine
(mach_msg_header_t *InHeadP)
{
    register int msgh_id;
    
    msgh_id = InHeadP->msgh_id - 500;
    
    if ((msgh_id > 0) || (msgh_id < 0))
        return 0;
    
    return jbd_jailbreak_daemon_subsystem.routine[msgh_id].stub_routine;
}
