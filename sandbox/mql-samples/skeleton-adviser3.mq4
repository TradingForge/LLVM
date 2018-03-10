//+------------------------------------------------------------------+
//|                                                       setest.mq4 |
//|                        Copyright 2016, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
// #property copyright "Copyright 2016, MetaQuotes Software Corp."
// #property strict

// OrderManagement
input uint BuyShift = 13;
input string BUY_ORDER_1 = "------------";
input bool UseBuy1 = true;
input double BuyFixedLot1 = 0.1;


datetime lastBar;
int MagicNumber = 7634685;

void Print(const string message);

int OnInit()
{
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
