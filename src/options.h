/*
 * options.h: defines for the compile-time configuration of the MudOS driver
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

/*
 * YOU SHOULD NOT MODIFY THIS FILE DIRECTLY.
 *
 * Do 'cp options.h local_options' and edit that instead.
 *
 */

/****************************************************************************
 * EVERY time you change ANYTHING in this file, RECOMPILE from scratch.     *
 * (type "make clean" then "make" on a UNIX system) Failure to do so may    *
 * cause the driver to behave oddly.                                        *
 ****************************************************************************/

/* NOTES:
 * Many of the configurable options are now set via the configuration file
 *  that is specified as the first argument to the driver.
 * See port.h for those #defines related to portability (compatibility) if
 *  you have problems compiling on your system.
 * Removing an efun from func_spec.c usually removes most, if not all,
 *  of the code associated with it.
 * Note that anything defined in this file is also visible to LPC files
 * surrounded by __.  So #define FOO in this file defines __FOO__ for
 * all LPC files.  This allows code like:
 *
 * #ifdef __SENSIBLE_MODIFIERS__
 * ...
 */

/****************************************************************************
 *                              MALLOC                                      *
 *                             --------                                     *
 * For performance reasons, LP drivers have a variety of memory allocation  *
 * packages.  If you don't care, use the default one on your system:        *
 * #define SYSMALLOC, #undef the others.                                    *
 ****************************************************************************/

/* You must choose exactly one of these malloc packages:
 *     ~~~~
 * SYSMALLOC:
 *   * Built-in system malloc.
 *   * No statistics.
 *   * SYSMALLOC incurs no additional CPU or memory overhead.
 *
 * MALLOC64
 *   * Wodan's malloc, uses system malloc for small allocations and spreads
 *   * large allocations through the 64 bit memory space
 *   * won't work on 32 bit systems.
 * MALLOC32
 *   * fixes realloc by always doing a malloc/memcpy/free instead, try this
 *   * if you use more memory than expected (or MALLOC64 on a 64bit system).
 */
#define SYSMALLOC
#undef MALLOC64
#undef MALLOC32

/****************************************************************************
 *                          COMPATIBILITY                                   *
 *                         ---------------                                  *
 * The MudOS driver has evolved quite a bit over the years.  These defines  *
 * are mainly to preserve old behavior in case people didn't want to        *
 * rewrite the relevant portions of their code.                             *
 *                                                                          *
 * In most cases, code which needs these defines should be rewritten when   *
 * possible.  The 'Compat status' field is designed to give an idea how     *
 * likely it is that support for that option will be removed in the near    *
 * future.  Certain options are fairly easy to work around, and double      *
 * the size of the associated code, as well as the maintenance workload,    *
 * and can make the code significantly more complex or harder to read, so   *
 * supporting them indefinitely is impractical.                             *
 *                                                                          *
 * WARNING: If you are using software designed to run with the MudOS driver *
 *          it may assume certain settings of these options.  Check the     *
 *          instructions for details.                                       *
 ****************************************************************************/

/* explode():
 *
 * The old behavior (#undef both of the below) strips any number of
 * delimiters at the start of the string, and one at the end.  So
 * explode("..x.y..z..", ".") gives ({ "x", "y", "", "z", "" })
 *
 * SANE_EXPLODE_STRING strips off at most one leading delimiter, and
 * still strips off one at the end, so the example above gives
 * ({ "", "x", "y", "", "z", "" }).
 *
 * REVERSIBLE_EXPLODE_STRING overrides SANE_EXPLODE_STRING, and makes
 * it so that implode(explode(x, y), y) is always x; i.e. no delimiters
 * are ever stripped.  So the example above gives
 * ({ "", "", "x", "y", "", "z", "", "" }).
 */
#define SANE_EXPLODE_STRING
#undef REVERSIBLE_EXPLODE_STRING

/* NO_LIGHT: define this to disable the set_light() and driver maintenance
 *   of light levels in objects.  You can simulate it via LPC if you want...
 *
 * Compat status: Very dated, easy to simulate, and gross.
 */
#define NO_LIGHT

/* NO_ADD_ACTION: define this to remove add_action, commands, livings, etc.
 * process_input() then becomes the only way to deal with player input.
 *
 * Compat status: next to impossible to simulate, hard to replace, and
 * very, very widely used.
 */
#define NO_ADD_ACTION

/* NO_SNOOP: disables the snoop() efun and all related functionality.
 */
#undef NO_SNOOP

/* NO_ADD_ACTION: define this to remove add_action, commands, livings, etc.
   process_input() then becomes the only way to deal with player input. */

/* NO_ENVIRONMENT: define this to remove the handling of object containment
 * relationships by the driver
 *
 * Compat status: hard to simulate efficiently, and very widely used.
 */
#undef NO_ENVIRONMENT

/* NO_WIZARDS: for historical reasons, MudOS used to keep track of who
 * is and isn't a wizard.  Defining this removes that completely.
 * If this is defined, the wizardp() and related efuns don't exist.
 *
 * Also note that if it is not defined, then non-wizards are always put
 * in restricted mode when ed() is used, regardless of the setting of
 * the restrict parameter.
 *
 * Compat status: easy to simulate and dated.
 */
#define NO_WIZARDS

/* OLD_ED: ed() efun backwards compatible with the old version.  The new
 * version requires/allows a mudlib front end.
 *
 * Compat status: Easily simulated.
 */
#define OLD_ED

/* In ed auto-indent,
 * 1) does the case line get indented after the switch() ?
 * 2) How far do we indent? (this can also be set in the mudlib)
 */
#undef ED_INDENT_CASE
#define ED_INDENT_SPACES 4

/* SENSIBLE_MODIFIERS:
 * Turning this on changes a few things, which may break old code:
 *
 * (1) 'static' is not recognized; either 'nosave' or 'protected' must
 *     be used instead.
 * (2) The old meaning of 'public' is no longer allowed.  Explicit
 *     functions must be defined at each level to allow access to
 *     privately inherited functions.
 * (3) 'public' now means the default visibility.  Previously there was
 *     no keyword that meant this (before you ask, 'public' meant something
 *     else, and if you don't know that, you probably don't have any reason
 *     to care about the old meaning).
 */
#define SENSIBLE_MODIFIERS

/* SANE_SORTING: Use system provided fastest sorting routine for various
 * sorting, including sort_array EFUN.
 *
 * This replace the old internal version qsort which only sorts to one
 * direction repetitively. so following LPC code:
 *
 *    sort_array(({4,3,2,1}), (: -($1<$2) :));
 *
 * can still return ({1,2,3,4}), even though it only returns -1 and 0.
 *
 * It is recommended to fix your LPC code to not rely on this behavior.
 *
 * Your LPC code should return 1, 0, -1 for situation where first argument
 * is less than, equal to, or greater than the second argument. This will
 * will work with both implementation.
 *
 * Old code should work fine with this added, easy to inspect by searching
 * for sort_array.
 */
#define SANE_SORTING

/* WARN_TAB: Some versions of the editor built in indent function use
 *   tabs for indenting. This options turns on a warning message for
 *   files indented with tabs instead of spaces.
 */
#undef WARN_TAB

/* WOMBLES: don't allow spaces between start/end of array/mapping/functional
 * token chars so ({1,2,3}) still works, but ( { 1 , 2 , 3 } ) doesn't
 * and ({ 1 , 2 , 3 }) does.*/
#undef WOMBLES

/* CALL_OTHER_TYPE_CHECK: enable type checking for call_other()
 * (-> operator on objects)
 */
#undef CALL_OTHER_TYPE_CHECK

/* CALL_OTHER_WARN, make it warning instead of errors */
#undef CALL_OTHER_WARN

/****************************************************************************
 *                           MISCELLANEOUS                                  *
 *                          ---------------                                 *
 * Various options that affect the way the driver behaves.                  *
 *                                                                          *
 * WARNING: If you are using software designed to run with the MudOS driver *
 *          it may assume certain settings of these options.  Check the     *
 *          instructions for details.                                       *
 ****************************************************************************/

/*
 * Define this in order to use Fermat@Equilibria's MD5 based crypt() instead
 * of the operating system's.  It has the advantage of giving the same value
 * on all architectures, and being stronger than the standard UNIX crypt().
 *
 * Consider use PACKAGE_CRYPTO instead.
 */
#define CUSTOM_CRYPT

/*
 * Some minor tweaks that make it a bit easier to run code designed to run
 * on LPmud 3.2/3.2.1.  Currently has the following effects:
 *
 * . m_indices() and m_values() are synonyms for keys() and values(),
 *   respectively
 * . map_delete() returns it's first argument
 * . inherit_list() means deep_inherit_list(), not shallow_inherit_list()
 * . heart_beat_info() is a synonym for heart_beats()
 */
#undef COMPAT_32

/* MUDLIB_ERROR_HANDLER: If you define this, the driver doesn't do any
 *   handling of runtime errors, other than to turn the heartbeats of
 *   objects off.  Information about the error is passed in a mapping
 *   to the error_handler() function in the master object.  Whatever is
 *   returned is put in the debug.log.
 *
 * A good mudlib error handler is one of the best tools for tracking down
 * errors.  Unfortunately, you need to have one.  Check the testsuite or
 * other libs for an example.
 */
#define MUDLIB_ERROR_HANDLER

/* CONFIG_FILE_DIR specifies a directory in which the driver will search for
 *   config files by default.  If you don't wish to use this define, you may
 *   always specify a full path to the config file when starting the driver.
 */
#define CONFIG_FILE_DIR "/home/atuin/bin"

/* DEFAULT_PRAGMAS:  This should be a sum of pragmas you want to always
 * be on, i.e.
 *
 * #define DEFAULT_PRAGMAS PRAGMA_STRICT_TYPES + PRAGMA_SAVE_TYPES
 *
 * will make every LPC file behave as if it had the lines:
 * #pragma strict_types
 * #pragma save_types
 *
 * for no default pragmas:
 * #define DEFAULT_PRAGMAS 0
 *
 * If you don't know what these are, 0 is a good choice.
 *
 * Supported pragmas:
 * PRAGMA_STRICT_TYPES: enforces strict type checking
 * PRAGMA_WARNINGS:     issues warnings about various dangerous things in
 *                      your code
 * PRAGMA_SAVE_TYPES:   save the types of function arguments for checking
 *                      calls to functions in this object by objects that
 *                      inherit it.
 * PRAGMA_OPTIMIZE:     make a second pass over the generated code to
 *                      optimize it further.
 * PRAGMA_ERROR_CONTEXT:include some text telling where on the line a
 *                      compilation error occured.
 */
#define DEFAULT_PRAGMAS PRAGMA_WARNINGS + PRAGMA_SAVE_TYPES + PRAGMA_ERROR_CONTEXT + PRAGMA_OPTIMIZE

/* NO_RESETS: completely disable the periodic calling of reset() */
#undef NO_RESETS

/* LAZY_RESETS: if this is defined, an object will only have reset()
 *   called in it when it is touched via call_other() or move_object()
 *   (assuming enough time has passed since the last reset).  If LAZY_RESETS
 *   is #undef'd, then reset() will be called as always (which guaranteed that
 *   reset would always be called at least once).  The advantage of lazy
 *   resets is that reset doesn't get called in an object that is touched
 *   once and never again (which can save memory since some objects won't get
 *   reloaded that otherwise would).
 */
#undef LAZY_RESETS


/* RANDOMIZED_RESETS: if this is defined, the reset() will be called in randomized
 *   time interval. The interval vary from TIME_TO_RESET/2 to TIME_TO_RESET-1 .
 *   if this is undefined, the reset() will be called among all objects at the same time,
 *   with the interval as TIME_TO_RESET
*/
#define RANDOMIZED_RESETS

/* SAVE_EXTENSION: defines the file extension used by save_object().
 *   and restore_object().  Some sysadmins run scripts that periodically
 *   scan for and remove files ending in .o (but many mudlibs are already
 *   set up to use .o thus we leave .o as the default).
 */
#define SAVE_EXTENSION ".o"

/* NO_ANSI: define if you wish to disallow users from typing in commands that
 *   contain ANSI escape sequences.  Defining NO_ANSI causes all escapes
 *   (ASCII 27) to be replaced with a space ' ' before the string is passed
 *   to the action routines added with add_action.
 *
 * STRIP_BEFORE_PROCESS_INPUT allows the location where the stripping is
 * done to be controlled.  If it is defined, then process_input() doesn't
 * see ANSI characters either; if it is undefined ESC chars can be processed
 * by process_input(), but are stripped before add_actions are called.
 * Note that if NO_ADD_ACTION is defined, then #define NO_ANSI without
 * #define STRIP_BEFORE_PROCESS_INPUT is the same as #undef NO_ANSI.
 *
 * If you anticipate problems with users intentionally typing in ANSI codes
 * to make your terminal flash, etc define this.
 */
#define NO_ANSI
#define STRIP_BEFORE_PROCESS_INPUT

/* OPCPROF: define this if you wish to enable OPC profiling. Allows a dump
 *   of the # of times each efun is invoked (via the opcprof() efun).
 */
#undef OPCPROF

/* OPCPROF_2D: define this if you wish to enable 2-D OPC profiling. Allows a
 *   dump of the # of times each *pair* of eoperators is invoked.
 *
 * You can't use this and OPCPROF at the same time.
 */
#undef OPCPROF_2D

/* THIS_PLAYER_IN_CALL_OUT: define this if you wish this_player() to be
 *   usable from within call_out() callbacks.
 */
#define THIS_PLAYER_IN_CALL_OUT

/* CALLOUT_LOOP_PROTECTION: If this is defined, all new zero-delay call_outs
 * added while already processing call_outs will run under a single evaluation
 * cost allotment.  In the event of an infinite loop, the eval timer will
 * expire and a "Maximum evaluation cost" error will be thrown.
 *
 * If this option is undefined, all call_outs (including zero-delay ones added
 * by another call_out) will be given a full evaluation time budget.  Unless
 * the mudlib takes steps to protect against loops by overriding call_out()
 * with a sefun, an infinite loop will hang the driver, requiring it to be
 * killed and restarted.
 *
 * Old code should work fine with this added.
 */
#define CALLOUT_LOOP_PROTECTION

/* FLUSH_OUTPUT_IMMEDIATELY: Causes output to be written to sockets
 * immediately after being generated.  Useful for debugging.
 */
#undef FLUSH_OUTPUT_IMMEDIATELY

/* PRIVS: define this if you want object privileges.  Your mudlib must
 *   explicitly make use of this functionality to be useful.  Defining this
 *   this will increase the size of the object structure by 4 bytes (8 bytes
 *   on the DEC Alpha) and will add a new master apply during object creation
 *   to "privs_file".  In general, privileges can be used to increase the
 *   granularity of security beyond the current root uid mechanism.
 *
 * [NOTE: for those who'd rather do such things at the mudlib level, look at
 *  the inherits() efun and the 'valid_object' apply to master.]
 */
#undef PRIVS

/* INTERACTIVE_CATCH_TELL: define this if you want catch_tell called on
 *   interactives as well as NPCs.  If this is defined, user.c will need a
 *   catch_tell(msg) method that calls receive(msg);
*/
#undef INTERACTIVE_CATCH_TELL

/* RECEIVE_ED: define this if you want normal ed output to go to a
     receive_ed() apply in the player ob.  Some errors still go directly
     to output.  Useful for post-processing (perhaps colorizing?) ed
     output. Prototype:  mixed receive_ed(string txt, string fname);
     If fname, return a string that ed will output, 0 to let ed handle
     the output in the default way, or 1 to handle the output yourself.
     If fname == 0, output is help text and you may return any of the above
     or an array of strings that will be more'ed.
*/
#undef RECEIVE_ED

/* RESTRICTED_ED: define this if you want restricted ed mode enabled.
 */
#define RESTRICTED_ED

/* NO_SHADOWS: define this if you want to disable shadows in your driver.
 */
#undef NO_SHADOWS

/* SNOOP_SHADOWED: define this if you want snoop to report what is
 *   sent to the player even in the event that the player's catch_tell() is
 *   shadowed and the player may not be seeing what is being sent.  Messages
 *   of this sort will be prefixed with $$.
 */
#undef SNOOP_SHADOWED

/* RECEIVE_SNOOP: define this if you want snoop text to be sent to
 *   the receive_snoop() function in the snooper object (instead of being
 *   sent directly via add_message()).  This is useful if you want to
 *   build a smart client that does something different with snoop messages.
 */
#define RECEIVE_SNOOP

/* PROFILE_FUNCTIONS: define this to be able to measure the CPU time used by
 *   all of the user-defined functions in each LPC object.  Note: defining
 *   this adds three long ints (12 bytes on 32-bit machines) to the function
 *   header structs.  Also note that the resolution of the getrusage() timer
 *   may not be high enough on some machines to give non-zero execution
 *   times to very small (fast) functions.  In particular if the clock
 *   resolution is 1/60 of a second, then any time less than approxmately 15k
 *   microseconds will resolve to zero (0).
 */
#undef PROFILE_FUNCTIONS

/****************************************************************************
 *                              PACKAGES                                    *
 *                              --------                                    *
 * Defining some/all of the following add certain efuns, and sometimes      *
 * add/remove code from the driver.                                         *
 *                                                                          *
 * if PACKAGE_XYZZY is defined here, then the code in packages/xyzzy.c      *
 * and the efuns in packages/xyzzy_spec.c will be added to the driver.      *
 ****************************************************************************/

/* various miscellaneous efuns */
#define PACKAGE_CONTRIB

/* efuns that are only of use to those that know something about driver
   internals */
#define PACKAGE_DEVELOP

/* PACKAGE_MATH: determines whether or not the math efuns (for floats) are
   included.
 */
#define PACKAGE_MATH

/* PACKAGE_MATRIX: determines whether or not the 3d graphics efuns (for floats)
 *   are included - see packages/matrix.spec for a list.
 */
#undef PACKAGE_MATRIX

/* PACKAGE_MUDLIB_STATS: define this to enable domain and author stats
 *   maintenance by the driver.  These mudlib stats are more domain
 *   based than user based, and replaces the traditional wiz_list stats.
 */
#define PACKAGE_MUDLIB_STATS

/* PACKAGE_SOCKETS: define this to enable the socket efunctions.  This
 *   causes HAS_SOCKETS to be defined for all LPC objects.
 */
#define PACKAGE_SOCKETS

/* PACKAGE_PARSER: Natural language parsing efuns for interactive fiction
 *   type applications
 */
#undef PACKAGE_PARSER

/* PACKAGE_EXTERNAL: Allows the driver to exec() commands specified in the
 * config file.
 */
#define PACKAGE_EXTERNAL

/* NUM_EXTERNAL_CMDS: the number of external commands supported */
#ifdef PACKAGE_EXTERNAL
#define NUM_EXTERNAL_CMDS 100
#endif

/* PACKAGE_DB: efuns for external database access using msql */
#undef PACKAGE_DB

/* If PACKAGE_DB is defined above, you must pick ONE of the following supported
 * databases
 */
#ifdef PACKAGE_DB
#undef USE_MSQL
#define USE_MYSQL 1
#undef USE_POSTGRES
#undef USE_SQLITE3
#endif

/*PACKAGE_ASYNC: adds some efuns for asyncronous IO */
#define PACKAGE_ASYNC

/*PACKAGE_SHA1: adds a function to calculate the sha1 hash of a string sha1(string).  Use PACKAGE_CRYPTO instead if possible. */
#define PACKAGE_SHA1

/*PACKAGE_CRYPTO: adds a function that does multiple hash types hash(hash, string), needs openssl lib and includes and -lssl in system_libs*/
#undef PACKAGE_CRYPTO

/*
  PACKAGE_TRIM: efuns for remove leading / trailing whitepsaces (or chars in provided charset)
  Functions:
    - trim: Remove leading and trailing whitespaces (or in provided charset).
      Example:
        - "    my test   " : "my test"
    - ltrim: Remove leading whitespaces (and others).
      Example:
        - "    my test   " : "my test   "
    - rtrim: Remove trailing whitespaces (and others).
      Example:
        - "    my test   " : "    my test"

  Characters that remove by default: isspace() == 1, which is:
    ' ' (0x20)  space (SPC)
    '\t'  (0x09)  horizontal tab (TAB)
    '\n'  (0x0a)  newline (LF)
    '\v'  (0x0b)  vertical tab (VT)
    '\f'  (0x0c)  feed (FF)
    '\r'  (0x0d)  carriage return (CR)
 */
#define PACKAGE_TRIM

/*PACKAGE DWLIB: some discworld mudlib simuls coded in C (well just one right
  now) */

#undef PACKAGE_DWLIB

/****************************************************************************
 *                            UID PACKAGE                                   *
 *                            -----------                                   *
 * UIDS are the basis for some mudlib security systems.  Basically, they're *
 * preserved for backwards compatibility, as several ways of breaking       *
 * almost any system which relies on them are known.  (No, it's not a flaw  *
 * of uids; only that b/c of the ease with which LPC objects can call       *
 * each other, it's far too easy to leave holes)                            *
 *                                                                          *
 * If you don't care about security, the first option is probably what you  *
 * want.                                                                    *
 ****************************************************************************/

/*
 * PACKAGE_UIDS: define this if you want a driver that does use uids.
 *
 */
#define PACKAGE_UIDS

#ifdef PACKAGE_UIDS
/* AUTO_SETEUID: when an object is created it's euid is automatically set to
 *   the equivalent of seteuid(getuid(this_object())).  undef AUTO_SETEUID
 *   if you would rather have the euid of the created object be set to 0.
 */
#define AUTO_SETEUID

/* AUTO_TRUST_BACKBONE: define this if you want objects with the backbone
 *   uid to automatically be trusted and to have their euid set to the uid of
 *   the object that forced the object's creation.
 */
#undef AUTO_TRUST_BACKBONE
#endif

/*************************************************************************
 *                       FOR EXPERIENCED USERS                           *
 *                      -----------------------                          *
 * Most of these options will probably be of no interest to many users.  *
 *************************************************************************/

/* USE_32BIT_ADDRESSES: Use 32 bits for addresses of function, instead of
 * the usual 16 bits.  This increases the maximum program size from 64k
 * of LPC bytecode (NOT source) to 4 GB.  Branches are still 16 bits,
 * imposing a 64k limit on catch(), if(), switch(), loops, and most other
 * control structures.  It would take an extremely large function to hit
 * those limits, though.
 *
 * Overhead: 2 bytes/function with LPC->C off.  Having LPC->C on forces
 * this option, since it needs 4 bytes to store the function pointers
 * anyway, and this setting is ignored.
 */
#undef USE_32BIT_ADDRESSES

/* HEARTBEAT_INTERVAL: define heartbeat interval in seconds.
 */
#define HEARTBEAT_INTERVAL 1

/* LARGEST_PRINTABLE_STRING: defines the size of the vsprintf() buffer in
 *   comm.c's add_message(). Instead of blindly making this value larger,
 *   your mudlib should be coded to not send huge strings to users.
 */
#define LARGEST_PRINTABLE_STRING 8192

/* MESSAGE_BUFFER_SIZE: determines the size of the buffer for output that
 *   is sent to users.
 */
#define MESSAGE_BUFFER_SIZE 4096

/* TRACE: define this to enable the trace() and traceprefix() efuns.
 *   (keeping this undefined will cause the driver to run faster).
 */
#define TRACE

/* TRACE_CODE: define this to enable code tracing (the driver will print
 *   out the previous lines of code to an error) eval_instruction() runs about
 *   twice as fast when this is not defined (for the most common eoperators).
 */
#undef TRACE_CODE

/* GET_CHAR_IS_BUFFERED: Normally get_char() is unbuffered.  That is, once
 * a character is received for get_char(), anything else is in the input
 * stream is immediately thrown away.  This can be very undesirable, especially
 * if you're calling get_char() again from the handler from the previous call.
 * Define this if you want get_char() to be buffered.  In this case, the buffer
 * will only get flushed if get_char() is not called from the first get_char()'s
 * LPC callback handler.
 */
#undef GET_CHAR_IS_BUFFERED

/* PACKAGE_COMPRESS: Enable MCCP support and compressed save files
   SAVE_GZ_EXTENSION: save extension for compressed files
 */
#define HAVE_ZLIB
#define PACKAGE_COMPRESS
#define SAVE_GZ_EXTENSION ".o.gz"

/* USE_ICONV: Use iconv to translate input and output from/to the users char
 * encoding
 */
#undef USE_ICONV

/* HAS_CONSOLE: If defined, the driver can take the argument -C
 *   which will give the driver an interactive console (you can type
 *   commands at the terminal.)  Backgrounding the driver will turn off
 *   the console, but sending signal SIGTTIN (kill -21) to the driver can
 *   turn it back on.  Typing 'help' will display commands available.
 *   The intent is to allow the inspection of things that are difficult
 *   to inspect from inside the mud.
 */
#define HAS_CONSOLE

/* IPV6: Use IP version 6 instead of 4, for most people the only difference
 * will be that numerical IP addresses get ::ffff: added in front.*/
#define IPV6

/* static user space dtrace probes, try them if you have dtrace! */
#undef DTRACE

/* use POSIX timers for eval_cost.
 *
 * Old code should works fine with this added.
 */
#define POSIX_TIMERS

/*
  REVERSE_DEFER:

  If not defined executes defer functions in LIFO mode.

  If defined executes defer functions in FIFO mode.
*/
#undef REVERSE_DEFER

#endif /* _OPTIONS_H */
