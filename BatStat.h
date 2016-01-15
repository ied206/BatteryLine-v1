#ifndef BATSTAT_H_INCLUDED
#define BATSTAT_H_INCLUDED

// https://msdn.microsoft.com/en-us/library/windows/desktop/aa373232%28v=vs.85%29.aspx
// http://www.codeproject.com/Articles/15829/Vista-Goodies-in-C-Monitoring-the-Computer-s-Power



/*
  BYTE  ACLineStatus;
The AC power status. This member can be one of the following values.
0	Offline
1	Online
255	Unknown status

  BYTE  BatteryFlag;
The battery charge status. This member can contain one or more of the following flags.
1	High-the battery capacity is at more than 66 percent
2	Low-the battery capacity is at less than 33 percent
4	Critical-the battery capacity is at less than five percent
8	Charging
128	No system battery
255	Unknown status-unable to read the battery flag information

  BYTE  BatteryLifePercent;
0-100, 255 : Unknown

듀얼모니터 대응
*/

/*
적용된 것 # 표시 지우기

[Setting]
showcharge   = true
#monitor      = primary
position     = top
taskbar      = ignore
transparency = 200
height       = 5

[Color]
defaultcolor = 0, 255, 0
chargecolor  = 0, 255, 196
color = 0, 20, 237, 28, 36
color = 20, 50, 255, 201, 14
*/

int BLBS_ReadSetting();
int BLBS_GetBatteryStat();
int IsPowerAC(SYSTEM_POWER_STATUS *stat);
int IsPowerCharging(SYSTEM_POWER_STATUS *stat);


#endif // BATSTAT_H_INCLUDED
