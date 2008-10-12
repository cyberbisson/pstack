
/** @file addr2ln.cpp
 ** @brief Contains main() for the addr2ln application.
 **
 ** @author  Matt Bisson
 ** @date    7 July, 2008
 ** @since   Addr2ln 0.1
 ** @version Addr2ln 0.1
 **/

#include <vector>

#include "addr2ln_ns.hpp"

#include <exec_file_module.hpp>

#include <exception/ui_exception.hpp>
#include <exception/windows_exception.hpp>

using namespace proclib;
using namespace psystem;

/** @todo We need an options class for addr2ln! */
const char *pn;
namespace proclib
{
    namespace demangle
    {
        namespace gcc
        {
            char *__cxa_demangle (const char *, char *, size_t *, int *);
        }
    }

    using namespace proclib::demangle::gcc;

}

//////////////////////////////////////////////////////////////////////////////
/// @name Macro Definitions
//////////////////////////////////////////////////////////////////////////////

/** @brief Print a simple description of an exception */
#define UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex)                              \
    fprintf (stderr, "%s: %s\n", pn,                        ex.what ()); \
    fflush (stdout);                                                    \
    fflush (stderr)

/** @brief Output an exception with "Too Much Information" (TMI) */
#define UI_TMI_OUTPUT_FOR_EXCEPTION(ex)                                 \
    fprintf (stderr, "%s: Exception caught at %s:%d (function %s): %s\n", \
             pn,                                                        \
             ex.source_file(), ex.line_number (),                       \
             ex.function(),    ex.what ());                             \
    fflush (stdout);                                                    \
    fflush (stderr)

/** @brief Depending on compilation settings, show exception output. */
#ifdef _DEBUG
#   define UI_OUTPUT_FOR_EXCEPTION(ex)       \
    UI_TMI_OUTPUT_FOR_EXCEPTION(ex)
#else
#   define UI_OUTPUT_FOR_EXCEPTION(ex)       \
    UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex)
#endif

/// @}

std::vector<memAddress_t> addrs;

int main (int argc, char *argv[])
{
    pn = argv[0];
    int m_ReturnCode = 0;
    try
    {
        if (argc < 2)
        {
            fprintf (stderr, "No symbol file given.\n");
        }

        /** @todo Assuming start address of 0x400000 */
        proclib::exec_file_module *efm = exec_file_module::Create (
            argv[1], (memAddress_t)0x400000);

        efm->init_or_throw ();

        /** @todo Need a real argument parser */
        for (int i = 2; i < argc; ++i)
        {
            addrs.push_back (strtoul (argv[i], NULL, 0));
        }

        char output[512];

        for (size_t i = 0; i < addrs.size (); ++i)
        {
            size_t size = 512;
            int error = 0;
            try
            {
                symbol_t sym = efm->getSymbol (addrs[i]);
                printf ("0x%08I64X ", addrs[i]);
                __cxa_demangle (sym.name, output, &size, &error);

                if (0 == error)
                {
                    printf (output);
                }
                else
                {
                    printf (sym.name);
                }

                printf (" [+0x%02I64X]\n", addrs[i] - sym.address);
            }
            catch (...)
            {
                printf ("0x%08I64X ??? ()\n", addrs[i]);
            }
        }

        efm->destroy ();
    }
    catch (psystem::exception::windows_exception& wex)
    {
        UI_OUTPUT_FOR_EXCEPTION(wex);
        m_ReturnCode = (int)wex.error_code ();
    }
    catch (psystem::exception::base_exception& bex)
    {
        UI_OUTPUT_FOR_EXCEPTION(bex);
        m_ReturnCode = -1;
    }
    catch (std::exception& ex)
    {
        UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex);
        m_ReturnCode = -2;
    }
    catch (...)
    {
        fprintf (stderr, "%s: An unknown exception occurred.\n",
                 pn);
        m_ReturnCode = -3;
    }

    return m_ReturnCode;
}
