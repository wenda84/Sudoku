#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define     BYTE                    unsigned char
#define     INVALID_BYTE            (BYTE)0xff    

#define     MAP_SIZE                9
#define     CNT_OF_VALUE_LIST       MAP_SIZE
#define     MAP_SUB_SIZE            3

#define     UNIT_STATUS_INIT        0
#define     UNIT_STATUS_CALCING     1
#define     UNIT_STATUS_CONFIRMED   2

#define     MAP_STATUS_INIT             0       /* ��ʼ�� */
#define     MAP_STATUS_UNIT_CALCING     1       /* �����������δ֪��Ԫ����ֵ */
#define     MAP_STATUS_UNIT_RECALING    2       /* ���е�Ԫ��ֵ�״μ�����Ϻ�,�������޷�����ȷ����ֵ,�ڽ��м��� */
#define     MAP_STATUS_GUESSING         3 
#define     MAP_STATUS_ALL_FINISH       4

#define     PROC_SUCCSS                 0
#define     PROC_FAIL_COMM              1       /* ����ʧ��: ͨ�ô��� */
#define     PROC_FAIL_NO_VALUE          2       /* ����ʧ��: ����Чֵ */

#define     PRINT_LINE                                           \
                do                                               \
                {                                                \
                    BYTE    bTmp;                                \
                    printf(" ");                                 \
                    for(bTmp = 0; bTmp < (MAP_SIZE*(MAP_SIZE-1) + 2); bTmp++) \
                    {                                            \
                        printf("-");                             \
                    }                                            \
                    printf(" \n");                               \
                }   while(0)


#define Q1  "....3..9....2....1.5.9..............1.2.8.4.6.8.5...2..75......4.1..6..3.....4.6."

/* easy */
#define E1  "5.1.9.4284.2..31...9...16.3....1.98..2.5.7.6..85.6....9.61...3...36..7.9247.3.5.6"
#define E2  ".5.81...98.2..71.5.6.3..7......7.2.3.1.5.6.8.3.7.9......5..3.1.1.87..5.66...25.74"
#define E3  "3.14...894....315..2.1..4...6....8347.3...5.2518....9...5..1.2..348....598...56.1"

/* med */
#define M1  "..98..24...49....1.1..6.......71...2.936.247.7...83.......4..2.9....51...52..87.."
#define M2  "..4.91.2.3.6...9.1.9..7.......98.765...7.6...867.15.......2..1.2.5...4.6.4.16.2.."
#define M3  ".392.6...6....542..4.87.....51...7..4.......1..2...86.....84.9..867....4...6.917."

/* hard */
#define H1  ".71.59..42.........4.7..6.....51.94..68...15..59.84.....2..5.1.........33..49.52."
#define H2  ".4...9....9.5623.....43...8..2.8...9..69.51..9...2.6..7...48.....1753.9....2...57"
#define H3  "....3...54.3.9726.69..2.....6.45..7.9.8...5.6.7..86.2.....1..43.2436.9.11...4...."

/* very hard */
#define V1  "...9..1.24..1.6....8..2...563....5....45.79....5....319...6..1....8.9..77.6..2..."
#define V2  "..6..1.8.17...9.6....467...61..4.8....2...3....5.7..96...215....3.6...28.2.7..6.5"
#define V3  "...7.5..8....2.59..4.3...2...396..72..62.19..29..378...1...3.5..67.1....3..8.9..."
#define V4  "..1..2.9....4..62..6..9.1.88...1594...98.65...2593...69.2.5..6..13..8....5.1..3.."

typedef struct tagT_unit_values_list
{
    BYTE    bCnt;

    BYTE    abValues[CNT_OF_VALUE_LIST];
}T_unit_values_list;

typedef struct tagT_unit
{
    BYTE                bValue;                     /* ����ȷ�ϵ�ֵ                        */
    BYTE                bUnitSatus;                 /* ״̬ -- 0:��ʼ��, 1:���ڼ��� 2:ȷ�� */
    
    T_unit_values_list  tMaybeValueList;            /* ��¼���ܵ���ֵ, Ӧ��unit״̬Ϊ1ʱ��Ч */
    BYTE                bInitFlag;                  /* �Ƿ���Ԥ����ֵ */
}T_unit;

typedef struct tagT_map
{
    T_unit  tUnit[MAP_SIZE][MAP_SIZE];

    BYTE    bMapStatus;
}T_map;

int     g_iCnt = 0;

BYTE calc_map(T_map* ptMap);

/* ��ӡ����                         */
/* bFuncFlag:   0 -- ���ַ�����ӡ   */
/*              1 -- ��9*9��ʽ��ӡ  */
void print_map(T_map* ptMap, BYTE bFuncFlag)
{
    BYTE    i,j,k;
    char    szChar[MAP_SIZE] = {0};
    char    szcharTmp[2]     = {0};
    if(0 == bFuncFlag)
    {
        ;
    }
    else if(1 ==  bFuncFlag)
    {
        PRINT_LINE   ;
        
        for(i = 0; i < MAP_SIZE; i++)
        {
            printf("|");
        
            for(j = 0; j < MAP_SIZE; j++)
            {
                if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[j][i].bUnitSatus)
                {
                    if(1 == ptMap->tUnit[j][i].bInitFlag)
                    {
                        printf("%7d*", ptMap->tUnit[j][i].bValue);    
                    }
                    else
                    {
                        printf("%8d", ptMap->tUnit[j][i].bValue);    
                    }            
                }
                else if(UNIT_STATUS_INIT == ptMap->tUnit[j][i].bUnitSatus)
                {
                    //printf("%8x", 0xff);                
                    printf("      --");
                }
                else if(UNIT_STATUS_CALCING == ptMap->tUnit[j][i].bUnitSatus)
                {
                    memset(szChar, 0x0, sizeof(szChar));
                
                    for(k = 0; k < CNT_OF_VALUE_LIST && k < ptMap->tUnit[j][i].tMaybeValueList.bCnt; k++)
                    {
                        memset(szcharTmp, 0x0, sizeof(szcharTmp));
                    
                        sprintf(szcharTmp, "%d", ptMap->tUnit[j][i].tMaybeValueList.abValues[k]);

                        strcat(szChar, szcharTmp);
                    }

                    printf("%8s", szChar);                
                }

                if((j+1)%MAP_SUB_SIZE == 0)
                {
                    printf("|");
                }
            }
            
            printf("\n");  

            if((i+1)%MAP_SUB_SIZE == 0)
            {
                PRINT_LINE;
            }
        }
    }
}

BYTE init_map(T_map* ptMap, const char* szChar)
{
    char    szCharTemp[MAP_SIZE*MAP_SIZE] = {0};
    BYTE    i,j;
    BYTE    bDoFlag = 1 ;
    memset(ptMap, 0x0, sizeof(T_map));
    
    /*  ������η�ֹ�ڴ�Խ��? */
    memcpy(szCharTemp, szChar, sizeof(szCharTemp));

    for(i = 0; i < MAP_SIZE && 1 == bDoFlag; i++)
    {
        for(j = 0; j < MAP_SIZE; j++)
        {
            if(0 == szCharTemp[i*MAP_SIZE+j]  )
            {
                bDoFlag = 0;
                break;
            }
            else if('.'  == szCharTemp[i*MAP_SIZE+j])
            {
                continue;
            }
            else if('1'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 1;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('2'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 2;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('3'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 3;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('4'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 4;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('5'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 5;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('6'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 6;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('7'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 7;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('8'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 8;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else if('9'  == szCharTemp[i*MAP_SIZE+j])
            {
                ptMap->tUnit[j][i].bValue       = 9;
                ptMap->tUnit[j][i].bUnitSatus   = UNIT_STATUS_CONFIRMED;
                ptMap->tUnit[j][i].bInitFlag    = 1;
            }
            else
            {
                printf("### invalid char!! ###\n");
                return PROC_FAIL_COMM;
            }
        }
    }

    printf("Import sudoku map succss, your map is:\n");

    print_map(ptMap, 1);

    return PROC_SUCCSS;
}

/* ��ptList��ɾ��bValue(Ĭ��list��ֻ����һ��bValue) */
BYTE  del_value_from_list(BYTE bValue, T_unit_values_list* ptList)
{
    BYTE        i,j ;
    BYTE        bDelCnt = 0;
    
    if(NULL == ptList)
    {
        return PROC_FAIL_COMM;
    }

    for(i = 0; i < ptList->bCnt && i < CNT_OF_VALUE_LIST; i++)
    {
        if(bValue != ptList->abValues[i])
        {
            continue;
        }

        if(1 == ptList->bCnt)
        {
            ptList->bCnt--;
            memset(ptList->abValues, 0x0, sizeof(ptList->abValues));

            return PROC_SUCCSS;
        }

        /* �Ѻ����ֵ��ǰ�� */
        for(j = i; j < (ptList->bCnt -1) && j < (CNT_OF_VALUE_LIST - 1); j++)
        {
            ptList->abValues[j] = ptList->abValues[j+1];
        }

        /* ɾ�����һ��ֵ */
        ptList->abValues[ptList->bCnt -1] = 0;
        ptList->bCnt--;

        return PROC_SUCCSS;
    }

    return PROC_SUCCSS;
}

BYTE calc_sig_unit(T_map* ptMap, BYTE x, BYTE y)
{
    BYTE        bOldStatus = INVALID_BYTE; 
    T_unit*     ptUnit     = NULL;
    BYTE        i,j;
    BYTE        bRet       = INVALID_BYTE;
    BYTE        bMinX,bMaxX,bMinY,bMaxY;
    
    if(x > (MAP_SIZE-1) || y > (MAP_SIZE-1))
    {
        return PROC_FAIL_COMM;
    }

    ptUnit = &ptMap->tUnit[x][y];
    if(NULL == ptUnit)
    {
        return PROC_FAIL_COMM;
    }

    //printf("=========== Now calc unit(%d,%d) ===========\n", x,y);
    //print_map(ptMap, 1);

    /* ���㱾unit����3*3�Ź������ʼ���� */
    bMinX = (x/MAP_SUB_SIZE)*MAP_SUB_SIZE;
    bMaxX = (x/MAP_SUB_SIZE)*MAP_SUB_SIZE + MAP_SUB_SIZE;
    bMinY = (y/MAP_SUB_SIZE)*MAP_SUB_SIZE;
    bMaxY = (y/MAP_SUB_SIZE)*MAP_SUB_SIZE + MAP_SUB_SIZE;

    /* step1. ��Ԫ������Ǽ���̬,תΪ����̬ */
    if(UNIT_STATUS_INIT == ptUnit->bUnitSatus)
    {
        bOldStatus         = ptUnit->bUnitSatus;
        
        ptUnit->bUnitSatus = UNIT_STATUS_CALCING;

        /* ��ʼ����ֵ���п��� */
        ptUnit->tMaybeValueList.bCnt = CNT_OF_VALUE_LIST;
        for(i = 0; i < CNT_OF_VALUE_LIST; i++)
        {
            ptUnit->tMaybeValueList.abValues[i] = i+1;
        }
    }
    
    /* step2. ����������,�ų��Ƿ�ֵ */
    if(UNIT_STATUS_CALCING == ptUnit->bUnitSatus)
    {
        for(i = 0; i < MAP_SIZE; i++)
        {
            /* �����Լ� */
            if( i == x )
            {
                continue;
            }

            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[i][y].bUnitSatus)
            {
                del_value_from_list(ptMap->tUnit[i][y].bValue, &ptUnit->tMaybeValueList);

                if(1 == ptUnit->tMaybeValueList.bCnt)
                {
                    ptUnit->bValue     = ptUnit->tMaybeValueList.abValues[0];
                    ptUnit->bUnitSatus = UNIT_STATUS_CONFIRMED;
                }

                if(0 == ptUnit->tMaybeValueList.bCnt)
                {
                    return PROC_FAIL_NO_VALUE;
                }
            }
        }
    }

    /* step3. ����������,�ų��Ƿ�ֵ */
    if(UNIT_STATUS_CALCING == ptUnit->bUnitSatus)
    {
        for(i = 0; i < MAP_SIZE; i++)
        {
            /* �����Լ� */
            if( i == y )
            {
                continue;
            }

            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][i].bUnitSatus)
            {
                del_value_from_list(ptMap->tUnit[x][i].bValue, &ptUnit->tMaybeValueList);

                if(1 == ptUnit->tMaybeValueList.bCnt)
                {
                    ptUnit->bValue     = ptUnit->tMaybeValueList.abValues[0];
                    ptUnit->bUnitSatus = UNIT_STATUS_CONFIRMED;
                }
                
                if(0 == ptUnit->tMaybeValueList.bCnt)
                {
                    return PROC_FAIL_NO_VALUE;
                }
            }
        }
    }

    /* step4. ��������3*3������,�ų��Ƿ�ֵ */
    if(UNIT_STATUS_CALCING == ptUnit->bUnitSatus)
    {
        for(i = bMinY; i < bMaxY; i++)
        {
            for(j = bMinX; j < bMaxX; j++)
            {
                /* �����Լ� */
                if(j == x && i == y)
                {
                    continue;
                }
            
                if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[j][i].bUnitSatus)
                {
                    del_value_from_list(ptMap->tUnit[j][i].bValue, &ptUnit->tMaybeValueList);

                    if(1 == ptUnit->tMaybeValueList.bCnt)
                    {
                        ptUnit->bValue     = ptUnit->tMaybeValueList.abValues[0];
                        ptUnit->bUnitSatus = UNIT_STATUS_CONFIRMED;
                    }
                    
                    if(0 == ptUnit->tMaybeValueList.bCnt)
                    {
                        return PROC_FAIL_NO_VALUE;
                    }
                }
            }
        }
    }

    /* step5. ע��:��ʼ�ݹ�!!!!                                                                      */
    /*        �������Ԫ�����ȷ��̬, �������ڼ���һ�α���Ԫ����������������е�Ԫ��ֵ               */
    if(UNIT_STATUS_CONFIRMED == ptUnit->bUnitSatus)
    {
        /* ������ */
        for(i = 0; i < MAP_SIZE; i++)
        {
            if(UNIT_STATUS_CALCING == ptMap->tUnit[x][i].bUnitSatus)
            {
                bRet = calc_sig_unit(ptMap, x, i);
                if( PROC_SUCCSS != bRet )
                {
                    //printf("### T_T, sub unit(%d,%d) calc failed! RetCode = %d\n",x,i,bRet);
                    return bRet;
                }
            }
        }
        
        /* ������ */
        for(i = 0; i < MAP_SIZE; i++)
        {
            if(UNIT_STATUS_CALCING == ptMap->tUnit[i][y].bUnitSatus)
            {
                bRet = calc_sig_unit(ptMap, i, y);
                if( PROC_SUCCSS != bRet )
                {
                    //printf("### T_T, sub unit(%d,%d) calc failed! RetCode = %d\n",i,y,bRet);
                    return bRet;
                }
            }        
        }

        /* �������������� */
        for(i = bMinY; i < bMaxY; i++)
        {
            for(j = bMinX; j < bMaxX; j++)
            {
                if(UNIT_STATUS_CALCING == ptMap->tUnit[j][i].bUnitSatus)
                {
                    bRet = calc_sig_unit(ptMap, j, i);
                    if( PROC_SUCCSS != bRet )
                    {
                        //printf("### T_T, sub unit(%d,%d) calc failed! RetCode = %d\n",j,i,bRet);
                        return bRet;
                    }
                }
            }
        }        

    /* �ݹ�ȫ������ */        
    }

    g_iCnt++;
    return PROC_SUCCSS;
}

BYTE is_map_calc_succ(T_map* ptMap)
{
    BYTE    i,j;
    BYTE    bCntConfirmed = 0;
    BYTE    bCntCalcing   = 0;
    BYTE    bCntInit      = 0;
    
    if(NULL == ptMap)
    {
        return PROC_FAIL_COMM;
    }    

    for(i = 0; i < MAP_SIZE; i++)
    {
        for(j = 0; j < MAP_SIZE; j++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[i][j].bUnitSatus)
            {
                bCntConfirmed++;
            }
            else if(UNIT_STATUS_CALCING == ptMap->tUnit[i][j].bUnitSatus)
            {
                bCntCalcing++;
            }
            else if(UNIT_STATUS_INIT == ptMap->tUnit[i][j].bUnitSatus)
            {
                bCntInit++;
            }
        }
    }

    if(MAP_SIZE*MAP_SIZE == bCntConfirmed)
    {
        ptMap->bMapStatus = MAP_STATUS_ALL_FINISH;

        return PROC_SUCCSS;
    }

    return PROC_FAIL_COMM;
}

/* ����һ���Ź�����ÿ�����ֵĿ��ܳ��ִ���,��������ֿ��ܳ��ִ���Ϊ1,��ȷ�������ڵ�Ԫ */
/* x,y:�Ź������ʼ����                                                              */
BYTE calc_sub_map(T_map* ptMap, BYTE x, BYTE y)
{
    BYTE        i,j,k;
    /* bCnt[1]��¼1��ĳ�������ڳ��ֵĴ���,������������. bCnt[0]��ʹ�� */
    BYTE    abValueCnt[10] = {0};
    BYTE    bValue;
    
    if(NULL == ptMap)
    {
        return PROC_FAIL_COMM;
    }

    for(i = x; i < x+3; i++)
    {
        for(j = y; j < y+3; j++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[i][j].bUnitSatus)
            {
                continue;
            }
            else if(UNIT_STATUS_CALCING == ptMap->tUnit[i][j].bUnitSatus)
            {
                for(k = 0; k < ptMap->tUnit[i][j].tMaybeValueList.bCnt && k < CNT_OF_VALUE_LIST; k++)
                {
                    bValue = ptMap->tUnit[i][j].tMaybeValueList.abValues[k];
                    if(bValue > 0 && bValue < 10)
                    {
                        abValueCnt[bValue]++;
                    }
                }
            }
        }
    }

    /* �����ֵ���ֵĴ���Ϊ1,���ҵ���ֵ���ڵ�Ԫ,�޸ĵ�Ԫ��ֵ */
    for(bValue = 1; bValue < 10; bValue++ )
    {
        if(1 != abValueCnt[bValue])
        {
            continue;
        }

        for(i = x; i < x+3; i++)
        {
            for(j = y; j < y+3; j++)
            {
                if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[i][j].bUnitSatus)
                {
                    continue;
                }
                else if(UNIT_STATUS_CALCING == ptMap->tUnit[i][j].bUnitSatus)
                {
                    for(k = 0; k < ptMap->tUnit[i][j].tMaybeValueList.bCnt && k < CNT_OF_VALUE_LIST; k++)
                    {
                        if(bValue == ptMap->tUnit[i][j].tMaybeValueList.abValues[k]) 
                        {
                            ptMap->tUnit[i][j].bValue = bValue;
                            ptMap->tUnit[i][j].bUnitSatus = UNIT_STATUS_CONFIRMED;

                            /* �ٴμ�����ص�Ԫ */
                            calc_sig_unit(ptMap, i, j);
                        }
                    }
                }
            }
        }
    }    

    return PROC_SUCCSS;
}


/* ����ÿ����Ԫ�Ŀ���ֵ,�������ֵ�ǺϷ�,���Լ���������Ԫ��ֵ */
BYTE  guess_map(T_map* ptMap)
{
    BYTE    x,y,i;
    T_map   tTmpMap;
    
    if(NULL == ptMap)
    {
        return PROC_FAIL_COMM;
    }

    //printf("now begin guess the map. \n");

    for(x = 0; x < MAP_SIZE; x++)
    {
        for(y = 0; y < MAP_SIZE; y++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
            {
                continue;
            }
            else if(UNIT_STATUS_CALCING == ptMap->tUnit[x][y].bUnitSatus)
            {
                for(i = 0; i < ptMap->tUnit[x][y].tMaybeValueList.bCnt && i < CNT_OF_VALUE_LIST; i++)
                {
                    memcpy(&tTmpMap, ptMap, sizeof(tTmpMap));

                    tTmpMap.tUnit[x][y].bUnitSatus = UNIT_STATUS_CONFIRMED;
                    tTmpMap.tUnit[x][y].bValue     = ptMap->tUnit[x][y].tMaybeValueList.abValues[i];

                    calc_map(&tTmpMap);
                    /* calc_sig_unit(&tTmpMap, x, y); */
                    if(PROC_SUCCSS == is_map_calc_succ(&tTmpMap))
                    {
                        memcpy(ptMap, &tTmpMap, sizeof(tTmpMap));
                    
                        return PROC_SUCCSS;
                    }
                }
            }
        }
    }

    return PROC_FAIL_COMM;
}

/* ������(��/��/�Ź���)��ͳ�ƿ���ֵ�ĳ��ִ���,�������ֵ���ִ���Ϊ1,���ȷ����ֵ��λ�� */
BYTE calc_map_by_area(T_map* ptMap)
{
    BYTE    x,y,i,j ;
    BYTE    bValue;
    /* bCnt[1]��¼1��ĳ�������ڳ��ֵĴ���,������������. bCnt[0]��ʹ�� */
    BYTE    abValueCnt[10] = {0};

    if(NULL == ptMap)
    {
        return PROC_FAIL_COMM;
    }

    /* step1. ���м��� */
    for(x = 0; x < MAP_SIZE; x++)
    {
        memset(abValueCnt, 0, sizeof(abValueCnt));

        /* ����ÿ��ֵ�ĳ��ִ��� */
        for(y = 0; y < MAP_SIZE; y++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
            {
                continue;
            }
            else if(UNIT_STATUS_CALCING == ptMap->tUnit[x][y].bUnitSatus)
            {
                for(i = 0; i < ptMap->tUnit[x][y].tMaybeValueList.bCnt && i < CNT_OF_VALUE_LIST; i++)
                {
                    bValue = ptMap->tUnit[x][y].tMaybeValueList.abValues[i];
                    if(bValue > 0 && bValue < 10)
                    {
                        abValueCnt[bValue]++;
                    }
                }
            }
        }

        /* �����ֵ���ֵĴ���Ϊ1,���ҵ���ֵ���ڵ�Ԫ,�޸ĵ�Ԫ��ֵ */
        for(i = 1; i < 10; i++ )
        {
            if(1 != abValueCnt[i])
            {
                continue;
            }

            for(y = 0; y < MAP_SIZE; y++)
            {
                if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
                {
                    continue;
                }
                else if(UNIT_STATUS_CALCING == ptMap->tUnit[x][y].bUnitSatus)
                {
                    for(j = 0; j < ptMap->tUnit[x][y].tMaybeValueList.bCnt && j < CNT_OF_VALUE_LIST; j++)
                    {
                        if(i == ptMap->tUnit[x][y].tMaybeValueList.abValues[j]) 
                        {
                            ptMap->tUnit[x][y].bValue = i;
                            ptMap->tUnit[x][y].bUnitSatus = UNIT_STATUS_CONFIRMED;

                            /* �ٴμ�����ص�Ԫ */
                            calc_sig_unit(ptMap, x, y);
                        }
                    }
                }

            }
        }
    }

    /* step2. ���м���  */
    for(y = 0; y < MAP_SIZE; y++)
    {
        memset(abValueCnt, 0, sizeof(abValueCnt));

        /* ����ÿ��ֵ�ĳ��ִ��� */
        for(x = 0; x < MAP_SIZE; x++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
            {
                continue;
            }
            else if(UNIT_STATUS_CALCING == ptMap->tUnit[x][y].bUnitSatus)
            {
                for(i = 0; i < ptMap->tUnit[x][y].tMaybeValueList.bCnt && i < CNT_OF_VALUE_LIST; i++)
                {
                    bValue = ptMap->tUnit[x][y].tMaybeValueList.abValues[i];
                    if(bValue > 0 && bValue < 10)
                    {
                        abValueCnt[bValue]++;
                    }
                }
            }
        }

        /* �����ֵ���ֵĴ���Ϊ1,���ҵ���ֵ���ڵ�Ԫ,�޸ĵ�Ԫ��ֵ */
        for(i = 1; i < 10; i++ )
        {
            if(1 != abValueCnt[i])
            {
                continue;
            }

            for(x = 0; x < MAP_SIZE; x++)
            {
                if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
                {
                    continue;
                }
                else if(UNIT_STATUS_CALCING == ptMap->tUnit[x][y].bUnitSatus)
                {
                    for(j = 0; j < ptMap->tUnit[x][y].tMaybeValueList.bCnt && j < CNT_OF_VALUE_LIST; j++)
                    {
                        if(i == ptMap->tUnit[x][y].tMaybeValueList.abValues[j]) 
                        {
                            ptMap->tUnit[x][y].bValue = i;
                            ptMap->tUnit[x][y].bUnitSatus = UNIT_STATUS_CONFIRMED;

                            /* �ٴμ�����ص�Ԫ */
                            calc_sig_unit(ptMap, x, y);
                        }
                    }
                }
            }
        }
    }

    /* step3. ����Ź������ */
    for(x = 0; x < MAP_SIZE; x = x+3)
    {
        for(y = 0; y < MAP_SIZE; y = y+3)
        {
            calc_sub_map(ptMap, x, y);
        }
    }

    return PROC_SUCCSS;
}

BYTE calc_map(T_map* ptMap)
{
    BYTE    bRet = INVALID_BYTE;
    BYTE    x,y;

    if(NULL == ptMap)
    {
        return PROC_FAIL_COMM;
    }
    
    /* step1. mapתΪ����̬ */
    ptMap->bMapStatus = MAP_STATUS_UNIT_CALCING;

    /* step2. �������unit״̬ */
    for(y = 0; y < MAP_SIZE; y++)
    {
        for(x = 0; x < MAP_SIZE; x++)
        {
            if(UNIT_STATUS_CONFIRMED == ptMap->tUnit[x][y].bUnitSatus)
            {
                continue;
            }
            
            bRet = calc_sig_unit(ptMap, x, y);
            if(PROC_SUCCSS != bRet  )
            {
                printf("### T_T, unit(%d,%d) calc failed! RetCode = %d\n",x,y,bRet);
                return bRet;
            }
        }
    }

    /* step3: ��Ԫ���μ������,�����ж���������������Ȼ�е�Ԫ���ڼ���̬, �������ʼ����ͳ�ﴦ�� */
    if(PROC_SUCCSS != is_map_calc_succ(ptMap))
    {
        ptMap->bMapStatus = MAP_STATUS_UNIT_RECALING;
        calc_map_by_area(ptMap);
    }

    /* step4, û���õİ취��,��ʼ�� */
    if(PROC_SUCCSS != is_map_calc_succ(ptMap))
    {
        ptMap->bMapStatus = MAP_STATUS_GUESSING;
        guess_map(ptMap);
    }

    /* step5: �ж��Ƿ������ */
    if(PROC_SUCCSS == is_map_calc_succ(ptMap))
    {
        ptMap->bMapStatus = MAP_STATUS_ALL_FINISH;
        printf("Congratulations! Map calc finish! Use %d steps.\n", g_iCnt);

        printf("====================== final result ======================\n");
        print_map(ptMap, 1);
    }
    else
    {
        printf("T_T, too hard for me, now status = %d. Use %d steps.\n", ptMap->bMapStatus, g_iCnt);
    }
    
    return PROC_SUCCSS;
}

void test_print_map()
{
    T_map   tMap;

    memset(&tMap, 0x0, sizeof(tMap));

    tMap.tUnit[0][2].bUnitSatus = UNIT_STATUS_CONFIRMED;
    tMap.tUnit[0][2].bValue     = 9;

    tMap.tUnit[0][1].bUnitSatus = UNIT_STATUS_CALCING;
    tMap.tUnit[0][1].tMaybeValueList.bCnt = 2;
    tMap.tUnit[0][1].tMaybeValueList.abValues[0] = 3;
    tMap.tUnit[0][1].tMaybeValueList.abValues[1] = 9;

    print_map(&tMap, 1);
}

void test_init_map()
{
    T_map   tMap;

    memset(&tMap, 0x0, sizeof(tMap));

    init_map(&tMap, "123456789..3.8....123456789");

    print_map(&tMap, 1);
}

void test_del_value_from_list()
{
    T_unit_values_list          tList;
    BYTE                        i;
    
    tList.bCnt          =  3;
    
    tList.abValues[0]   =  2;
    tList.abValues[1]   =  3;
    tList.abValues[2]   =  9;

    for(i = 0; i < tList.bCnt; i++)
    {
        printf("%d", tList.abValues[i]);
    }

    PRINT_LINE;
    
    del_value_from_list(2, &tList);

    for(i = 0; i < tList.bCnt; i++)
    {
        printf("%d", tList.abValues[i]);
    }

    return ;
    
}

void test_main()
{
    T_map tMap;

    init_map(&tMap, V4);

    calc_map(&tMap);
    
    return ;
}

void print_usage()
{
    T_map tMap;
    
    printf("for example: u can use this string(%s) to create a example sudoku table. \r\n", E1);
    
    init_map(&tMap, E1);
    
    return;
}


void calc_input()
{
    T_map tMap;

    char szString[MAP_SIZE*MAP_SIZE+1] = {0};    

    print_usage();

    printf("please input a sudoku string now:\n");
    
    scanf("%s", szString);

    if(PROC_SUCCSS == init_map(&tMap, szString))
    {
        calc_map(&tMap);
    }

#ifdef _WIN32
    system("pause");
#endif

    return ;
}
int main()
{
    calc_input();
    return 0;
}



