/*******************************************/
/* LanListP procedure                      */
/*                                         */
/* Syntax: LanListP C/S [dd/mm/yyyy]       */
/*                                         */
/* Here:                                   */
/* C/S - use Common/Separate Logs          */
/* dd/mm/yyyy - date                       */
/*******************************************/

Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
Call SysLoadFuncs

parse upper arg Log DateP

if DateP='' then do
  Date=DATE('S')
  DateP=substr(Date,7,2)||'/'||substr(Date,5,2)||'/'||substr(Date,1,4)
end
else Date=substr(DateP,7,4)||substr(DateP,4,2)||substr(DateP,1,2)

call SysFileDelete 'LanList.In'
call SysFileDelete 'LanList.Out'
call SysFileDelete 'LanList.Srt'

if Log='S' then do
  FM='LanScan-'||Date||'*.Log'
  call SysFileTree FM, 'File', 'OF'
  if File.0 = 0 then do
    say 'File' FM 'not found'
    exit
  end

  '@copy' File.1 'LanList.In>nul'
  do i = 2 to File.0
    '@copy LanList.In+'File.i 'LanList.In>nul'
  end
end

else do
  InFile='LanScan.Log'
  call SysFileTree InFile, 'File', 'OF'
  if File.0 = 0 then do
    say 'File' InFile 'not found'
    exit
  end
  '@copy' InFile 'LanList.In>nul'
end

'@sort<LanList.In>LanList.Srt'

'@LanList' DateP

'@start netscape file://'||Directory()||'\LanList.htm'

/* If you would like to use Mozilla, comment out the line above */
/* and uncomment 2 lines below.                                 */

/* HtmlPath=translate(Directory()||'/LanList.htm', '/', '\')    */
/* '@start mozilla file://'HtmlPath                             */
