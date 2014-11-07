//=============================================================================
// LanList.c 1.1
// Программа построения отчета о функционировании узлов сети IP.
// В качестве входных данных используется Log программы LanScan
//=============================================================================
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libc\sys\stat.h>

#define MaxLen   256
#define ReadLen  240
#define MaxWidth 480

void main(int argc, char *argv[])
{
  FILE *File, *FileHtm;
  char Line[MaxLen], LineDate[16], LineTime[16];
  BOOL ScanFlag = FALSE;
  char *TablePtr, *ScanPtr, *IPaddrPtr;
  char *TableP,   *ScanP,   *IPaddrP;
  int NumScan = 0, NumIP = 0;
  int i, j, len, SumLen;
  char IPaddress[16] = "";
  char FileSrt[] = "LanList.Srt", FileIn[] = "LanList.In";
  char StartTime[9], StopTime[9], StartInAct[9], StopInAct[9];
  struct stat info;

//-----------------------------------------------------------------------------
// Проверим параметры
//-----------------------------------------------------------------------------
  if ( argc < 2 )
    { printf("Syntax: LanList dd/mm/yyyy\n"); return; }
//-----------------------------------------------------------------------------
// Посчитаем количество сканирований и IP-адресов
//-----------------------------------------------------------------------------
  if ( stat(FileSrt, &info) != 0 )
    { printf("File %s not found\n", FileSrt); return; }
  if ( (int)info.st_size == 0 )
    { printf("Size of file %s = 0.\n\n", FileSrt);
      printf("Please, check input file for SORT LanList.In.\n");
      printf("It's possible, LanList.In is bigger 64K.\n");
      printf("If so, use program BIGSORT (bigsrt44.zip) from HOBBES.\n");
      return;
    }

  if ( (File = fopen(FileSrt, "r")) == NULL )
    { printf("Could not open file %s\n", FileSrt); return; }

  while ( fgets(Line, ReadLen, File) != NULL )
    {
    if ( strncmp(Line, "LanScan", 7) == 0 )
      {
      sscanf(Line, "%*s %*s %s %*s", LineDate);
      if ( strcmp(LineDate, argv[1] ) == 0 )
        {
        NumScan++;
        continue;
        }
      }

    if ( isdigit(*Line) && (strncmp(Line, IPaddress, 15) != 0) )
      {
      strncpy(IPaddress, Line, 15);
      IPaddress[15] = '\0';
      NumIP++;
      }
    }
//-----------------------------------------------------------------------------
// Получим ОП
//-----------------------------------------------------------------------------
  if ( NumScan == 0 )
    { printf("Number of scanning = 0. Please check file %s\n", FileSrt);
      return;
    }
  if ( NumIP == 0 )
    { printf("Number of IP addresses = 0. Please check file %s\n", FileSrt);
      return;
    }

  if ( (ScanPtr = calloc(NumScan, 9)) == NULL )
    { printf("calloc error for NumScan\n"); return; }
  if ( (IPaddrPtr = calloc(NumIP, 16)) == NULL )
    { printf("calloc error for NumIP\n"); return; }
  if ( (TablePtr = calloc(NumIP*NumScan, 1)) == NULL )
    { printf("calloc error for NumScan*NumIP\n"); return; }
//-----------------------------------------------------------------------------
// Заполним массивы "время сканирования" и "IP-адрес"
//-----------------------------------------------------------------------------
  ScanP = ScanPtr;
  IPaddrP = IPaddrPtr;
  IPaddress[0] = '\0';

  rewind(File);
  while ( fgets(Line, ReadLen, File) != NULL )
    {
    if ( strncmp(Line, "LanScan", 7) == 0 )
      {
      sscanf(Line, "%*s %*s %s %s", LineDate, ScanP);
      if ( strcmp(LineDate, argv[1]) == 0 )
        {
        ScanP += 9;
        continue;
        }
      }

    if ( isdigit(*Line) && (strncmp(Line, IPaddress, 15) != 0) )
      {
      strncpy(IPaddress, Line, 15);
      strncpy(IPaddrP, Line, 15);
      IPaddrP += 16;
      }
    }

  fclose(File);
//-----------------------------------------------------------------------------
// Заполним массив Table "IP-адрес найден во время сканирования"
//-----------------------------------------------------------------------------
  if ( (File = fopen(FileIn, "r")) == NULL )
    { printf("File %s not found\n", FileIn); return; }

  while ( fgets(Line, ReadLen, File) != NULL )
    {
    if ( strncmp(Line, "LanScan", 7) == 0 )
      {
      sscanf(Line, "%*s %*s %s %s", LineDate, LineTime);
      if ( strcmp(LineDate, argv[1]) != 0 )
        {
        ScanFlag = FALSE;
        continue;
        }

      for (i=0, ScanP=ScanPtr; i<NumScan; i++, ScanP+=9)
        {
        if ( strcmp(LineTime, ScanP) == 0 )
          {
          ScanFlag = TRUE;
          break;
          }
        }
      continue;
      }

    if ( isdigit(*Line) && ScanFlag )
      {
      for (j=0, IPaddrP=IPaddrPtr; j<NumIP; j++, IPaddrP+=16)
        {
        if ( strncmp(Line, IPaddrP, 15) == 0 )
          {
          *(TablePtr+j*NumScan+i) = TRUE;
          break;
          }
        }
      }
    }

  fclose(File);
//-----------------------------------------------------------------------------
// Выводим в заголовок в LanList.htm
//-----------------------------------------------------------------------------
  if ( (FileHtm = fopen("LanList.htm", "w")) == NULL )
    { printf("Could not open file LanList.hut\n"); return; }

  fprintf(FileHtm, "<html>\n");
  fprintf(FileHtm, "<head>\n");
  fprintf(FileHtm, "<title>LAN Activity Diagram %s</title>\n", argv[1]);
  fprintf(FileHtm, "</head>\n");
  fprintf(FileHtm, "<body>\n");
  fprintf(FileHtm, "<table bgcolor=\"#FFFFFF\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n");
  fprintf(FileHtm, "<tr>\n");
  fprintf(FileHtm, "<td>&nbsp;</td>\n");
  fprintf(FileHtm, "<td width=\"482\">\n");
  fprintf(FileHtm, "<table bgcolor=\"#FFFFFF\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
  fprintf(FileHtm, "<tr>\n");
  for (i=0; i<24; i++) fprintf(FileHtm, "<td width=\"20\" align=\"left\">%d</td>\n", i);
  fprintf(FileHtm, "</tr>\n");
  fprintf(FileHtm, "</table>\n");
  fprintf(FileHtm, "</td>\n");
  fprintf(FileHtm, "</tr>\n");
//-----------------------------------------------------------------------------
// Выводим в LanList.Out массив Table "IP-адрес найден во время сканирования"
//-----------------------------------------------------------------------------
  if ( (File = fopen("LanList.Out", "w")) == NULL )
    { printf("Could not open file LanList.Out\n"); return; }

  fprintf(File, "LanList 1.0  %s\n", argv[1]);

  for (i=0, IPaddrP=IPaddrPtr; i<NumIP; i++, IPaddrP+=16)
    {
    SumLen = 0;
    strcpy(StartTime, "99:99:99");
    strcpy(StopTime, "00:00:00");
    strcpy(StartInAct, "00:00:00");

    fprintf(File, "%s", IPaddrP);
    fprintf(FileHtm, "<tr>\n");
    memset(IPaddress, '\0', sizeof(IPaddress));
    for (j=0; (j<15) && (*(IPaddrP+j)!=' '); j++) IPaddress[j]=*(IPaddrP+j);
    fprintf(FileHtm, "<td>%s</td>\n", IPaddress);
    fprintf(FileHtm, "<td width=\"482\" background=\"scale.gif\" valign=\"center\">\n");

    for (j=0, ScanP=ScanPtr; j<NumScan; j++, ScanP+=9)
      {
      if ( *(TablePtr+i*NumScan+j) )
        {
        if ( strcmp(StartTime, ScanP) > 0 ) strcpy(StartTime, ScanP);
        if ( strcmp(StopTime, ScanP) < 0 ) strcpy(StopTime, ScanP);
        continue;
        }
      if ( StartTime[0] != '9' )
        {
        fprintf(File, "  %s-%s",StartTime,StopTime);

        memcpy(StopInAct, StartTime, 6);
        StartInAct[2] = '\0';
        StartInAct[5] = '\0';
        StopInAct[2]  = '\0';
        StopInAct[5]  = '\0';
        len=(60*(atoi(StopInAct)-atoi(StartInAct))+atoi(StopInAct+3)-atoi(StartInAct+3))/3;
        if ( len != 0 ) fprintf(FileHtm, "<img src=\"white.gif\" width=\"%d\" height=\"10\" border=\"0\" align=\"center\">", len);
        SumLen += len;
        memcpy(StartInAct, StopTime, 6);

        StartTime[2] = '\0';
        StartTime[5] = '\0';
        StopTime[2]  = '\0';
        StopTime[5]  = '\0';
        len=(60*(atoi(StopTime)-atoi(StartTime))+atoi(StopTime+3)-atoi(StartTime+3))/3;
        if ( len != 0 ) fprintf(FileHtm, "<img src=\"blue.gif\" width=\"%d\" height=\"10\" border=\"0\" align=\"center\">", len);
        SumLen += len;
        }
      strcpy(StartTime, "99:99:99");
      strcpy(StopTime, "00:00:00");
      }
    if ( StartTime[0] != '9' )
      {
      fprintf(File, "  %s-%s",StartTime,StopTime);

      memcpy(StopInAct, StartTime, 6);
      StartInAct[2] = '\0';
      StartInAct[5] = '\0';
      StopInAct[2]  = '\0';
      StopInAct[5]  = '\0';
      len=(60*(atoi(StopInAct)-atoi(StartInAct))+atoi(StopInAct+3)-atoi(StartInAct+3))/3;
      if ( len != 0 ) fprintf(FileHtm, "<img src=\"white.gif\" width=\"%d\" height=\"10\" border=\"0\" align=\"center\">", len);
      SumLen +=len;

      StartTime[2] = '\0';
      StartTime[5] = '\0';
      StopTime[2] = '\0';
      StopTime[5] = '\0';
      len=(60*(atoi(StopTime)-atoi(StartTime))+atoi(StopTime+3)-atoi(StartTime+3))/3;
      if ( len != 0 ) fprintf(FileHtm, "<img src=\"blue.gif\" width=\"%d\" height=\"10\" border=\"0\" align=\"center\">", len);
      SumLen += len;
      }
    len=MaxWidth-SumLen;
    if ( len != 0 ) fprintf(FileHtm, "<img src=\"white.gif\" width=\"%d\" height=\"10\" border=\"0\" align=\"center\">", len);
    fprintf(File, "\n");
    fprintf(FileHtm, "</td></tr>\n");
    }

  fclose(File);

  fprintf(FileHtm, "<tr>\n");
  fprintf(FileHtm, "<td>&nbsp;</td>\n");
  fprintf(FileHtm, "<td width=\"482\">\n");
  fprintf(FileHtm, "<table bgcolor=\"#FFFFFF\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
  fprintf(FileHtm, "<tr>\n");
  for (i=0; i<24; i++) fprintf(FileHtm, "<td width=\"20\" align=\"left\">%d</td>\n", i);
  fprintf(FileHtm, "</tr>\n");
  fprintf(FileHtm, "</table>\n");
  fprintf(FileHtm, "</td>\n");
  fprintf(FileHtm, "</tr>\n");
  fprintf(FileHtm, "</table>\n");
  fprintf(FileHtm, "</body>\n");
  fprintf(FileHtm, "</html>\n");
  fclose(FileHtm);

  return ;
}
