//+------------------------------------------------------------------+
//|                                                       setest.mq4 |
//|                        Copyright 2016, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2016, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict

// OrderManagement
input uint BuyShift = 13;
input uint SellShift = 17;
input uint StartTraingHour = 2;
input uint EndTradingHour = 16;
input uint Slippage = 0;
input uint PriceShift = 3;

input string BUY_ORDER_1 = "------------";
input bool UseBuy1 = true;
input uint BuyOffset1 = 37;
input double BuyFixedLot1 = 0.1;
input uint BuyTP1 = 200;
input uint BuySL1 = 250;
input uint BuyExtremumOffset1 = 0;
input uint BuyThreshold = 100;
input uint BuyTrailingStop = 200;

input string SELL_ORDER_1 = "------------";
input bool UseSell1 = true;
input uint SellOffset1 = 100;
input double SellFixedLot1 = 0.1;
input uint SellTP1 = 200;
input uint SellSL1 = 250;
input uint SellExtremumOffset1 = 0;
input uint SellThreshold = 100;
input uint SellTrailingStop = 200;



//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+

datetime lastBar;
int buyTicket1 = -1;
int sellTicket1 = -1;
bool IsBuyPositionOpened = false;
bool IsSellPositionOpened = false;
int MagicNumber = 7634685;

int OnInit()
{
    Print("OnInit");
    lastBar = Time[0];
    EventSetTimer(60);

    return (INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    Print("OnDeinit: reason == ", reason);
    EventKillTimer();
}
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
    Print("OnTick");
}
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{
    Print("OnTimer");
}
//+------------------------------------------------------------------+
//| Tester function                                                  |
//+------------------------------------------------------------------+
double OnTester()
{
    Print("OnTester");

    double ret = 0.0;
    return (ret);
}
//+------------------------------------------------------------------+

void CheckOrders()
{
}

bool IsNewBar()
{
    return false;
}

void TrailingStops()
{
}

bool IsStopLossPositive()
{
    return false;
}

bool AllowTradesByTime()
{
    return false;
}

void CloseAllOrders()
{
}
