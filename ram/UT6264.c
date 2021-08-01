/* -----------------------------------------------------------------------------
 * Project      : 第五届集创赛信诺达杯数字芯片测试           
 * Chip         : UT6264CPC-70LL                        
 * File         : UT6264.c                              
 * Author       : ECUST-APEX                            
 * -----------------------------------------------------------------------------
 * Description  : 基于ST3020集成电路测试系统与mdc图形文件 
 * 的测试程序，芯片型号：UT6264CPC-70LL 
 * ----------------------------------------------------------------------------- 
 * Version      : 3.3
 * -------------------------------------------------------------------------- */

#include "StdAfx.h"
#include "testdef.h"
#include "data.h"
#include "math.h"

// ---- mdc ----
#define MDC_ILO 0
#define MDC_NUM_ILO 15
#define MDC_ICC 15
#define MDC_ICC12_LOW 16
#define MDC_ICC12_HIGH 17
#define MDC_SET0000 18
#define MDC_CLR0000 19
#define MDC_ISB 20
#define MDC_NUM_ISB 3
#define MDC_ISB1 23
#define MDC_NUM_ISB1 6
#define MDC_SET_ALL 29
#define MDC_CLR_ALL 30
#define MDC_WALK_METHOD 31
#define MDC_READ_ZERO 32
#define MDC_READ_ONE 33
#define MDC_CLOSE 34
#define MDC_WALK_METHOD 35

// ---- typ. ----
#define TYP_VSS 0.0

#define MIN_VCC 4.5
#define TYP_VCC 5.0
#define MAX_VCC 5.5

#define MIN_VIH 2.2
#define MAX_VIL 0.8

#define MIN_VOH 2.4
#define MAX_VOL 0.4

#define MIN_VDR 2.0
#define MAX_VDR 5.5

#define TYP_DELAY 5
#define TYP_LONG_DELAY 20

#define DELTA_V 0.2
// ---- channel ----
#define CHANNEL_ADDR "1-13"
#define CHANNEL_DATA "33-40"
#define CHANNEL_EN "44,45,47,48"
#define CHANNEL_ALL "1-13,33-40,44,45,47,48"
#define CHANNEL_IN "1-13,44,45,47,48"

// ---- period ----
#define TYP_PERIOD 75
#define TYP_TIME1 5
#define TYP_TIME2 50
#define TYP_TIME3 70

#define PERIOD_ICC 70
#define PERIOD_ICC1 1000
#define PERIOD_ICC2 500

// ---- function ----
#define SET_TYP_INPUT_LEVEL SET_INPUT_LEVEL(MIN_VIH, MAX_VIL)
#define SET_TYP_OUTPUT_LEVEL SET_OUTPUT_LEVEL(MIN_VOH, MAX_VOL)
#define SET_TYP_TIMING SET_TIMING(TYP_TIME1, TYP_TIME2, TYP_TIME3)
#define SET_TYP_PERIOD SET_PERIOD(TYP_PERIOD);

void SET_TYP()
{
    SET_TYP_INPUT_LEVEL;
    SET_TYP_OUTPUT_LEVEL;
    SET_TYP_TIMING;
    SET_TYP_PERIOD;
    FORMAT(NRZ0, CHANNEL_ALL);
}

void PASCAL UT62()
{
    int i = 0,cnt = 0; // for-loop
    double V_il = MAX_VIL, V_ih = MIN_VIH;
    // ---- global ----
    SET_TYP();
    // Note: 为了最大程度发挥全局属性的继承性,
    //       如果某测试单元修改了以上属性， 要在该测试单元结束后复原.

    // -------------------------------------------------------------------------
    // #1
    // PARAMETER: Connection 连接性测试
    // SYMBOL   : CON
    // CHANNEL  : ALL
    // CONDITION: TODO
    // PATTERN  : NULL
    // ---------------------------------
    SET_DPS(DPS1, TYP_VSS, V, 30, MA); //VCC连接DPS1,0V
    PMU_CONDITIONS(FIMV, -0.1, MA, 2, V);
    //测试结果会显示名称+对应通道号(CONx)
    if (!PMU_MEASURE(CHANNEL_ALL, TYP_DELAY, "CON_", V, -0.1, -1.2)) //-0.6~-0.7
        BIN(1);
    //-------------------------------------------------------------------------
    //#2
    // PARAMETER: Input HIGH/LOW Voltage
    // SYMBOL   : VIH/VIL
    // CHANNEL  : NULL
    // CONDITION: NULL
    // PATTERN  : SET0000/CLR0000
    //-------------------------------------------------------------------------
    cnt = (TYP_VCC+0.5 - MIN_VIH) / 0.1 + 1;
    for ( i = 0; i < cnt; i++)
    {
        SET_INPUT_LEVEL(V_ih,0.8);
        V_ih = V_ih + 0.1;
        if(!RUN_PATTERN(MDC_SET0000,1,0,0))
            BIN(2);
    }

    cnt = (MAX_VIL + 0.5) / 0.1 + 1;
    for ( i = 0; i < cnt; i++)
    {
        SET_INPUT_LEVEL(2.2,V_il);
        V_il = V_il - 0.1;
        if(!RUN_PATTERN(MDC_CLR0000,1,0,0))
            BIN(2);
    }

    // -------------------------------------------------------------------------
    // #3
    // PARAMETER: Input Leakage Current
    // SYMBOL   : ILI
    // CHANNEL  : IN
    // CONDITION: Vss <= V_IN <= Vcc(Vss = 0V, Vcc = 5V)
    // PATTERN  : NULL
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 50, MA);
    PMU_CONDITIONS(FVMI, TYP_VSS, V, 0.001, MA);
    if (!PMU_MEASURE(CHANNEL_IN, TYP_DELAY, "ILI_L_", UA, 1, -1))
        BIN(3);
    PMU_CONDITIONS(FVMI, TYP_VCC, V, 0.001, MA);
    if (!PMU_MEASURE(CHANNEL_IN, TYP_DELAY, "ILI_H_", UA, 1, -1))
        BIN(3);

    // -------------------------------------------------------------------------
    // #4
    // PARAMETER: Output Leakage Current
    // SYMBOL   : ILO
    // CHANNEL  : DATA
    // CONDITION: Vss <= V_I/O <= Vcc; nCE1 = V_IH; or CE2 = V_IL; or nOE = V_IH;
    // or nWE = V_IL
    // PATTERN  : ILO
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 50, MA);
    PMU_CONDITIONS(FVMI, TYP_VSS, V, 0.001, MA);
    for (i = MDC_ILO; i < MDC_ILO + MDC_NUM_ILO; i++)
    {
        RUN_PATTERN(i, 1, 0, 0);
        if (!PMU_MEASURE(CHANNEL_DATA, TYP_DELAY, "ILO_L_", 1, -1))
            BIN(4);
    }
    PMU_CONDITIONS(FVMI, TYP_VCC, V, 0.001, MA);
    for (i = MDC_ILO; i < MDC_ILO + MDC_NUM_ILO; i++)
    {
        RUN_PATTERN(i, 1, 0, 0);
        if (!PMU_MEASURE(CHANNEL_DATA, TYP_DELAY, "ILO_H_", UA, 1, -1))
            BIN(4);
    }

    // -------------------------------------------------------------------------
    // #5
    // PARAMETER: Output High Voltage
    // SYMBOL   : VOH
    // CHANNEL  : DATA
    // CONDITION: IOH = -1mA
    // PATTERN  : SET0000
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 45, MA);
    RUN_PATTERN("SET0000_", MDC_SET0000, 1, 0, 0);
    PMU_CONDITIONS(FIMV, -1, MA, 4.5, V);
    if (!PMU_MEASURE(CHANNEL_DATA, TYP_DELAY, "VOH_", V, No_UpLimit, 2.4))
        BIN(5);

    // -------------------------------------------------------------------------
    // #6
    // PARAMETER: Output Low Voltage
    // SYMBOL   : VOL
    // CHANNEL  : DATA
    // CONDITION: IOH = 4mA
    // PATTERN  : CLR0000
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 45, MA);
    RUN_PATTERN(MDC_CLR0000,1, 0, 0); //输出位于低电平
    PMU_CONDITIONS(FIMV, 4, MA, 0.4, V);
    if (!PMU_MEASURE(CHANNEL_DATA, TYP_DELAY, "VOL_", V, 0.4, No_LoLimit))
        BIN(6);

    // -------------------------------------------------------------------------
    // #7
    // PARAMETER: Operating Power Supply Current
    // SYMBOL   : Icc/Icc1/Icc2
    // CHANNEL  : DPS1
    // CONDITION: Cycle time = min/1US/500NS; I_I/O = 0mA; nCE1 = V_IL/0.2V/*;
    //            CE2 = V_IH/Vcc-0.2V/*; other pins at any/0.2v or Vcc-0.2V/*
    // ---------------------------------
    // Icc
    // PATTERN  : ICC
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 45, MA);
    SET_PERIOD(PERIOD_ICC);
    SET_TIMING(5, 50, 65);
    RUN_PATTERN(MDC_ICC, 1, 0, 0);
    if (!DPS_MEASURE(DPS1, R200MA, TYP_LONG_DELAY, "ICC_", MA, 45, No_LoLimit))
        BIN(7);
    // ---------------------------------
    // Icc1
    // PATTERN  : ICC12_HIGH, ICC12_LOW
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 30, MA);
    SET_PERIOD(PERIOD_ICC1); //通过图形文件模拟cycle time
    SET_TIMING(100, 600, 800);
    SET_INPUT_LEVEL(TYP_VCC - DELTA_V, DELTA_V); //VCC-0.2/0.2

    RUN_PATTERN(MDC_ICC12_HIGH, 1, 0, 0);
    if (!DPS_MEASURE(DPS1, R200MA, 5, "ICC1_H_", MA, 30, No_LoLimit))
        BIN(7);
    RUN_PATTERN(MDC_ICC12_LOW, 1, 0, 0);
    if (!DPS_MEASURE(DPS1, R200MA, 5, "ICC1_L_", MA, 30, No_LoLimit))
        BIN(7);
    // ---------------------------------
    // Icc2
    // PATTERN  : ICC12_HIGH, ICC12_LOW
    // ---------------------------------
    SET_DPS(DPS1, TYP_VCC, V, 15, MA);
    SET_PERIOD(PERIOD_ICC2);
    SET_TIMING(50, 300, 450);
    SET_INPUT_LEVEL(TYP_VCC - DELTA_V, DELTA_V); //VCC-0.2/0.2

    RUN_PATTERN(MDC_ICC12_HIGH, 1, 0, 0);
    if (!DPS_MEASURE(DPS1, R20MA, 5, "ICC2_", MA, 15, No_LoLimit))
        BIN(7);
    RUN_PATTERN(MDC_ICC12_LOW, 1, 0, 0);
    if (!DPS_MEASURE(DPS1, R20MA, 5, "ICC2_", MA, 15, No_LoLimit))
        BIN(7);

    SET_TYP(); // finally
    // -------------------------------------------------------------------------
    // #8
    // PARAMETER: Standby Current(TTL/CMOS)
    // SYMBOL   : ISB/ISB1
    // CHANNEL  : DPS1
    // CONDITION:
    // - nCE1 = V_IH or CE2 = V_IL
    // - nCE1 >= Vcc-0.2V or CE2 <= 0.2V; other pins at 0.2V or Vcc-0.2V
    // ---------------------------------
    // ISB
    // PATTERN  : ISB
    // ---------------------------------
    SET_DPS(DPS1, 5, V, 5, MA);
    for (i = MDC_ISB; i < MDC_ISB + MDC_NUM_ISB; i++)
    {
        RUN_PATTERN(i, 1, 0, 0); //CE1=VIH
        if (!DPS_MEASURE(1, R20MA, TYP_DELAY, "ISB_", MA, 3, No_LoLimit))
            BIN(8);
    }
    // ---------------------------------
    // ISB1
    // PATTERN  : ISB1
    // ---------------------------------
    SET_DPS(DPS1, 5, V, 70, UA);
    SET_INPUT_LEVEL(TYP_VCC - DELTA_V, DELTA_V); //VCC-0.2/0.2
    for (i = MDC_ISB1; i < MDC_ISB1 + MDC_NUM_ISB1; i++)
    {
        RUN_PATTERN(i, 1, 0, 0); //CE1=VIH
        if (!DPS_MEASURE(1, R200UA, 5, "ISB1_", UA, 50, No_LoLimit))
            BIN(8);
    }

    SET_TYP(); // finally
    // -------------------------------------------------------------------------
    // #9
    // PARAMETER: FUNCTION - WALK METHOD
    // SYMBOL   : FUN
    // CHANNEL  : NULL
    // CONDITION: NULL
    // PATTERN  : CLR_ALL, WALK_METHOD
    // ---------------------------------
    SET_DPS(DPS1, MIN_VCC, V, 100, MA);
    RUN_PATTERN(MDC_CLR_ALL, 1, 2, 0);
    if (!RUN_PATTERN("FUN_", MDC_WALK_METHOD, 1, 2, 0))
        BIN(9);
    //SET_DPS(DPS1, MAX_VCC, V, 100, MA); //VCC_max
    //RUN_PATTERN(MDC_CLR_ALL, 1, 2, 0);  //数据清0
    //if (!RUN_PATTERN("FUN_", MDC_WALK_METHOD, 1, 2, 0))
    //    BIN(9);
    SET_DPS(DPS1, MIN_VCC, V, 100, MA);
    RUN_PATTERN(MDC_SET_ALL, 1, 2, 0);
    if (!RUN_PATTERN("FUN_", MDC_WALK_METHOD2, 1, 2, 0))
        BIN(9);
    //SET_DPS(DPS1, MAX_VCC, V, 100, MA); //VCC_max
    //RUN_PATTERN(MDC_SET_ALL, 1, 2, 0);  //数据清0
    //if (!RUN_PATTERN("FUN_", MDC_WALK_METHOD2, 1, 2, 0))
    //    BIN(9);
    // -------------------------------------------------------------------------
    // #10
    // PARAMETER: FUNCTION - DATA RETENTION
    // SYMBOL   : DR0/DR1
    // CHANNEL  : NULL
    // CONDITION: nCE1 >= Vcc-0.2V or CE2 <= 0.2V
    // PATTERN  : CLR_ALL, READ_ZERO; SET_ALL, READ_ONE
    // ---------------------------------
    //由于VCC参数在功能测试中已经测试过，除非特殊要求，一般都设VCC=5V
    //VDR_min测试
    //测试内存能否存0
    SET_DPS(DPS1, TYP_VCC, V, 100, MA);

    RUN_PATTERN(MDC_CLR_ALL, 1, 2, 0); //写0

    SET_DPS(DPS1, MIN_VDR, V, 100, MA); //VDRmin
    SET_INPUT_LEVEL(MIN_VDR - DELTA_V, DELTA_V);          //输入电平

    RUN_PATTERN(MDC_CLOSE, 1, 0, 0); //CE端设置
    Delay(1000);                    //保持数据10s

    SET_DPS(DPS1, TYP_VCC, V, 100, MA);
    SET_TYP_INPUT_LEVEL;
    if (!RUN_PATTERN("DR0", MDC_READ_ZERO, 1, 2, 0))
        BIN(10);
    // -----------------------------
    //测试内存能否存1
    SET_DPS(DPS1, TYP_VCC, V, 100, MA);

    RUN_PATTERN(MDC_SET_ALL, 1, 2, 0); //写1

    SET_DPS(DPS1, MIN_VDR, V, 100, MA); //VDRmin
    SET_INPUT_LEVEL(MIN_VDR - DELTA_V, DELTA_V);          //输入电平

    RUN_PATTERN(MDC_CLOSE, 1, 0, 0); //CE端设置
    Delay(1000);                    //保持数据10s

    SET_DPS(DPS1, TYP_VCC, V, 100, MA);
    SET_TYP_INPUT_LEVEL;
    if (!RUN_PATTERN("DR1", MDC_READ_ONE, 1, 2, 0))
        BIN(10);
    // -----------------------------
    //VDR_max测试
    //测试内存能否存0
    SET_DPS(DPS1, TYP_VCC, V, 100, MA);

    RUN_PATTERN(MDC_CLR_ALL, 1, 2, 0); //写0

    SET_DPS(DPS1, MAX_VDR, V, 100, MA); //VDRmax
    SET_INPUT_LEVEL(MAX_VDR - DELTA_V, DELTA_V);          //输入电平

    RUN_PATTERN(MDC_CLOSE, 1, 0, 0); //设置CE端
    Delay(1000);                    //保持数据10s

    SET_DPS(DPS1, TYP_VCC, V, 100, MA);
    SET_TYP_INPUT_LEVEL;
    if (!RUN_PATTERN("DR0", MDC_READ_ZERO, 1, 2, 0))
        BIN(10);
    // -----------------------------
    //测试内存能否存1
    SET_DPS(DPS1, TYP_VCC, V, 100, MA);

    RUN_PATTERN(MDC_SET_ALL, 1, 2, 0); //写1

    SET_DPS(DPS1, MAX_VDR, V, 100, MA);  //VDRmax
    SET_INPUT_LEVEL(MAX_VDR - DELTA_V, DELTA_V); //输入电平

    RUN_PATTERN(MDC_CLOSE, 1, 0, 0); //设置CE端
    Delay(1000);                    //保持数据10s

    SET_DPS(DPS1, TYP_VCC, V, 100, MA);
    SET_TYP_INPUT_LEVEL;
    if (!RUN_PATTERN("DR1", MDC_READ_ONE, 1, 2, 0))
        BIN(10);

    // -------------------------------------------------------------------------
    // #11
    // PARAMETER: Data Retention Current
    // SYMBOL   : IDR
    // CHANNEL  : SOURCE
    // CONDITION: Vcc = 2V; nCE1 >= Vcc-0.2V or CE2 <= 0.2V
    // PATTERN  : CLR_ALL, READ_ZERO; SET_ALL, READ_ONE
    // ---------------------------------
    SET_DPS(DPS1, MIN_VDR, V, 20, UA);     //VCC=2V
    SET_INPUT_LEVEL(MIN_VDR - DELTA_V, DELTA_V);       //输入电平
    RUN_PATTERN(MDC_CLOSE, 1, 0, 0); //设置CE端
    if (!DPS_MEASURE(DPS1, R20UA, TYP_DELAY, "IDR", UA, 20, No_LoLimit))
        BIN(11);

    // SET_TYP() // finally
    // CLEAR_ALL();
}