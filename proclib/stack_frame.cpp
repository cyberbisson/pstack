
/** @file stack_frame.cpp
 ** @brief Declare objects that encapsulate a single stack frame.
 **
 ** @author  Matt Bisson
 ** @date    24 December, 2007
 ** @since   Proclib 1.1
 ** @version Proclib 1.3
 **/

#include <windows.h>

#include <dbghelp.hpp>
#include <process_trace.hpp>
#include <process_module.hpp>
#include <thread_trace.hpp>

#include <exception/null_pointer_exception.hpp>

#include "stack_frame.hpp"

//////////////////////////////////////////////////////////////////////////////
// Module typedefs, statics, and constants:
//////////////////////////////////////////////////////////////////////////////

/** @brief A list of the registers that we care about.
 **/
typedef enum
{
    REG_PROGRAM_COUNTER = 0,            /**< @brief PC */
    REG_FRAME_POINTER   = 1,            /**< @brief FP */
    REG_STACK_POINTER   = 2,            /**< @brief SP */
    REG_BACKING_STORE   = 3             /**< @brief Similar to FP (for Itanium)*/
} register_t;

/** @brief The process architectures we understand.
 **/
typedef enum
{
    ARCH_X86       = 0,                 /**< @brief Intel x86 */
    ARCH_INTEL_IPF = 1,                 /**< @brief Intel Itanium */
    ARCH_X64       = 2                  /**< @brief Intel x64 */
} architecture_t;

/** @brief Cache away the architecture type for this process.
 ** @todo This needs to be moved into process_module and exposed via
 **       process_trace.*/
static architecture_t g_Architecture = ARCH_X86;

/** Thus far, this matrix contains the x86, Intel Itanium Processor Family (IPF),
 ** and x64 registry names for program counter, frame pointer, stack pointer,
 ** and backing store.
 **
 ** This information is indexed using the architecture_t and register_t
 ** enumerations.  Therefore, if you need to get the name of the IPF's backing
 ** store, you would refer to s_RegNames[REG_BACKING_STORE][ARCH_INTEL_IPF].
 **
 ** @brief Mapping for processor type to a string containing register names.
 **/
const static char *s_RegNames[][3] =
{
    // x86,   Intel IPF, x64
    { "EIP",  "StIIP",   "RIP" },       // Program counter
    { "EBP",  "N/A",     "RBP/RDI" },   // Frame pointer
    { "ESP",  "SP",      "RSP" },       // Stack pointer
    { "N/A",  "RsBSP",   "N/A" }        // Backing store
};

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

proclib::stack_frame *proclib::stack_frame::Create (
        const proclib::thread_trace& thread, const STACKFRAME64& framedata)
        throw (std::bad_alloc)
{
    proclib::stack_frame *ret = new stack_frame (thread, framedata);

    if (NULL == ret) throw std::bad_alloc ();

    /** @todo Another non-lazy init() */
    ret->init_or_throw ();
    return ret;
}

/** @brief Find the code module where the program counter sits.
 **
 ** @return This should never return NULL.
 ** @throw null_pointer_exception If we don't actually know the module.
 **/
const char *proclib::stack_frame::getModuleName () const
    throw (psystem::exception::null_pointer_exception)
{
    if (!haveRelatedModule ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return m_RelatedModule->getModuleName().c_str ();
}

/** @brief Return a reference to the module containing our program counter.
 **
 ** @throw null_pointer_exception If we don't actually know the module.
 **/
const proclib::process_module& proclib::stack_frame::getModule () const
    throw (psystem::exception::null_pointer_exception)
{
    if (!haveRelatedModule ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return *m_RelatedModule;
}

/** @brief Return the line in the source file that corresponds to this frame.
 **
 ** @return If we don't have the line number, this function returns
 **         stack_frame::NO_LINE.
 ** @throw null_pointer_exception If we don't know the runtime module.
 **/
proclib::stack_frame::line_t proclib::stack_frame::getSourceLine () const
    throw (psystem::exception::null_pointer_exception)
{
    if (!haveRelatedModule ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return m_SourceLine;
}

/** @brief Find the name of the source code file that generated this frame's
 **        code.
 **
 ** @return This should never return NULL.  If we don't know the source file
 **         name, this will return an empty string.
 ** @throw null_pointer_exception If we don't actually know the runtime module.
 **/
const char *proclib::stack_frame::getSourceFile () const
    throw (psystem::exception::null_pointer_exception)
{
    if (!haveRelatedModule ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return m_SourceFile.c_str ();
}

/** @brief Return the reference to the process owning this stack frame.
 **/
const proclib::process_trace& proclib::stack_frame::getParentProcess () const
    throw ()
{
    return getParentThread().getParentProcess ();
}

//////////////////////////////////////////////////////////////////////////////
// Construction / destruction:
//////////////////////////////////////////////////////////////////////////////

proclib::stack_frame::stack_frame (
    const proclib::thread_trace& thread, const STACKFRAME64& framedata)
    throw ()
    : trace_object (INVALID_HANDLE_VALUE),
      m_FunctionName  (NULL),
      m_CodeOffset    (0),
      m_RelatedModule (NULL),
      m_SourceLine    (NO_LINE),
      m_StackFrame    (framedata),
      m_ParentThread  (thread)
{

}

//////////////////////////////////////////////////////////////////////////////
// Inheritable object interface:
//////////////////////////////////////////////////////////////////////////////

void proclib::stack_frame::dump_object () throw ()
{
    printf ("Dumping stack frame:\n");

    /** @todo Make a function that outputs with a hard stop on the number of
     **       characters in the first column of output. */
    printf ("\tProgram Counter (%s): 0x%08I64X\n",
            s_RegNames[REG_PROGRAM_COUNTER][g_Architecture],
            m_StackFrame.AddrPC.Offset);

    printf ("\tReturn Address:        0x%08I64X\n", m_StackFrame.AddrReturn.Offset);

    if (ARCH_INTEL_IPF != g_Architecture)
    {
        printf ("\tFrame Pointer (%s):   0x%08I64X\n",
                s_RegNames[REG_FRAME_POINTER][g_Architecture],
                m_StackFrame.AddrFrame.Offset);
    }
    else
    {
        printf ("\tBacking Store (%s): 0x%08I64X\n",
                s_RegNames[REG_BACKING_STORE][g_Architecture],
                m_StackFrame.AddrBStore.Offset);
    }

    printf ("\tStack Pointer (%s):   0x%08I64X\n",
            s_RegNames[REG_STACK_POINTER][g_Architecture],
            m_StackFrame.AddrStack.Offset);

    printf ("\tWOW Call:              %s\n",
            m_StackFrame.Far ? "true" : "false");
    printf ("\tVirtual Frame:         %s\n",
            m_StackFrame.Virtual ? "true" : "false");

    if ((ARCH_X86 == g_Architecture) && (NULL != m_StackFrame.FuncTableEntry))
    {
        // !!! Also can come from SymFunctionTableAccess64 
        const FPO_DATA *fpo_data = (FPO_DATA *)m_StackFrame.FuncTableEntry;

        printf ("\tFrame Pointer Omission (FPO) Data:\n");

        printf ("\t\tFrame Type:            0x%08X (", fpo_data->cbFrame);
        switch (fpo_data->cbFrame)
        {
        case FRAME_FPO:
            printf ("FPO Frame");
            break;
        case FRAME_NONFPO:
            printf ("Non-FPO Frame");
            break;
        case FRAME_TRAP:
            printf ("Trap Frame");
            break;
        case FRAME_TSS:
            printf ("TSS Frame");
            break;
        default:
            printf ("UNKNOWN");
            break;
        }
        printf (")\n");

        printf ("\t\tFunction Code Offset:  0x%08X\n", fpo_data->ulOffStart);
        printf ("\t\tLocal Variable Count:  %d\n", fpo_data->cdwLocals);
        printf ("\t\tFunction Size:         %d bytes\n", fpo_data->cbProcSize);
        printf ("\t\tParameter Size:        %d bytes\n",
                fpo_data->ulOffStart * sizeof (DWORD));
        printf ("\t\tFunction Prolog:       %d bytes\n", fpo_data->cbProlog);
        printf ("\t\tSaved Registers:       %d\n", fpo_data->cbRegs);
        printf ("\t\tUses Structure Exception Handling (SEH) - %s\n",
                fpo_data->fHasSEH ? "true" : "false");
        printf ("\t\tUses EBP register - %s\n",
                fpo_data->fUseBP ? "true" : "false");
    }

    /** @todo PARAMS */
}

void proclib::stack_frame::init () throw ()
{
    /** @todo Parent process is const! */
    proclib::process_trace& kProc =
        const_cast<proclib::process_trace&> (getParentProcess ());

    try
    {
        /** @todo Maybe we have some kind of listener model that gets rid of this
         **       reference when the underlying process_module dies? */
        m_RelatedModule = &(kProc.findModule (m_StackFrame.AddrPC.Offset));
    }
    catch (...)
    {
        /** @todo If I throw here, I might catch an exception I don't really
         **       care about... */
    }

    try
    {
        const symbol_t& symbol = kProc.findSymbol (m_StackFrame.AddrPC.Offset);
        m_CodeOffset = m_StackFrame.AddrPC.Offset - symbol.address;
        m_FunctionName = symbol.name;
    }
    catch (const char *)
    {
        //
    }

    DWORD           lineOffset = 0;
    IMAGEHLP_LINE64 imgHelp;

    memset (&imgHelp, 0, sizeof (imgHelp));
    imgHelp.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    if (psystem::dbghelp::SymGetLineFromAddr64 (
            kProc.getProcessHandle (),
            m_StackFrame.AddrPC.Offset,
            &lineOffset, &imgHelp))
    {
        if (NULL != imgHelp.FileName) m_SourceFile = imgHelp.FileName;
        m_SourceLine = (stack_frame::line_t)imgHelp.LineNumber;
        if (0 == m_CodeOffset) m_CodeOffset = lineOffset;
    }
    else
    {
        /** @todo ?? */
    }
}

void proclib::stack_frame::shutdown () throw ()
{

}
