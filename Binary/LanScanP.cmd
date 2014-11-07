/*******************************************/
/* LanScanP procedure                      */
/*                                         */
/* Syntax: LanScanP C/S                    */
/*                                         */
/* Here:                                   */
/* C/S - use Common/Separate Logs          */
/*******************************************/

parse arg Log

'start /C /MIN LanListP' Log

exit
