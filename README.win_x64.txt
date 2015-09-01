This is a build of version 1.0 of udf_infusion for 64-bit MySQL 5.1+.
It is licensed under the terms of the GNU General Public License
version 2. See the LICENSE file.

IMPORTANT NOTE: If you are running 32-bit MySQL, then you need the 32-bit version
of udf_infusion, not the 64-bit version.

All source code for this project is downloadable from GitHub:
https://github.com/infusion/udf_infusion


INSTALLATION
================================================================================
To install the udf_infusion UDFs:

 1. Make sure that you are running 64-bit Windows and that you have installed
    the 64-bit build of MySQL 5.1 or later.

    Note: The udf_infusion.dll binary has only been tested with MySQL version
    5.7, but should work with MySQL 5.6, 5.5, 5.1 and 5.0 as well. It is, however,
    incompatible with MySQL 3.23/4.0/4.1.

 2. Look for a file named msvcr120.dll in your System32 folder. If this file is
    not present, then install the latest Visual C++ Redistributable for Visual Studio 2013
    vcredist_x64.exe:
    https://www.microsoft.com/en-us/download/details.aspx?id=40784

 3. Copy udf_infusion.dll to the MySQL installation's lib\plugin folder.

    With a default installation, this is C:\Program Files\MySQL\MySQL Server 5.7\lib\plugin\

    Note: You do NOT need to re-start MySQL after copying udf_infusion.dll
    into the plugin directory.

 4. As root, source load.win.sql or simply paste the contents into the CLI client.


TROUBLESHOOTING
================================================================================
  * ERROR 1126 (HY000): Can't open shared library 'udf_infusion' (errno: 0 )
    Or,
    ERROR 1126 (HY000): Can't open shared library 'udf_infusion.dll' (errno: 126 The specified module could not be found.)

    This either means that the Visual C++ Redistributable for Visual Studio 2013
    is not installed or that MySQL could not find udf_infusion.dll in the
    plugin directory.

    Look for a file named msvcr120.dll in your System32 folder. If this file is
    not present, then install the latest Visual C++ Redistributable for Visual Studio 2013
    vcredist_x64.exe.

    Verify that udf_infusion.dll was copied into the plugin directory by
    executing:
    SHOW VARIABLES LIKE 'plugin_dir';

    The result should be:
    +---------------+-----------------------------------------------------+
    | Variable_name | Value                                               |
    +---------------+-----------------------------------------------------+
    | plugin_dir    | C:\Program Files\MySQL\MySQL Server 5.7\lib\plugin\ |
    +---------------+-----------------------------------------------------+

    If it's not, then copy udf_infusion.dll to the listed directory.

  * ERROR 1126 (HY000): Can't open shared library 'udf_infusion.dll' (errno: 193 )

    This error can happen when you try to use the 64-bit version of udf_infusion
    with a 32-bit MySQL server. Either use the 32-bit version of udf_infusion
    or install 64-bit MySQL.

  * ERROR 1127 (HY000): Can't find symbol in library

    This means that MySQL was able to find udf_infusion.dll, but for some
    reason, it could not "see" the UDF in the DLL module.

    If you are typing the CREATE FUNCTION statements manually, make sure that
    you have typed the UDF name exactly as listed in load.win.sql. For example,
    instead of "GROUP_FIRST", you must type "group_first" (all lowercase).


If you encounter any other problem, please feel free to contact me at:
    dtrebbien@gmail.com
