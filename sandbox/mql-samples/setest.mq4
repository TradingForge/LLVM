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
input uint   BuyShift            = 13;
input uint   SellShift           = 17;
input uint   StartTraingHour     = 2;
input uint   EndTradingHour      = 16;
input uint   Slippage            = 0;
input uint   PriceShift          = 3;

input string BUY_ORDER_1         = "------------";
input bool   UseBuy1             = true;
input uint   BuyOffset1          = 37;
input double BuyFixedLot1        = 0.1;
input uint   BuyTP1              = 200;
input uint   BuySL1              = 250;
input uint   BuyExtremumOffset1  = 0;
input uint   BuyThreshold        = 100;
input uint   BuyTrailingStop     = 200;

input string SELL_ORDER_1        = "------------";
input bool   UseSell1            = true;
input uint   SellOffset1         = 100;
input double SellFixedLot1       = 0.1;
input uint   SellTP1             = 200;
input uint   SellSL1             = 250;
input uint   SellExtremumOffset1 = 0;
input uint   SellThreshold       = 100;
input uint   SellTrailingStop    = 200;



//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+

datetime lastBar;
int      buyTicket1           = -1;
int      sellTicket1          = -1;
bool     IsBuyPositionOpened  = false;
bool     IsSellPositionOpened = false;
int      MagicNumber          = 7634685;

int OnInit()
{    
   lastBar = Time[0];
   EventSetTimer(60); 
     
   return(INIT_SUCCEEDED);
}
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   EventKillTimer();   
}
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   TrailingStops();
   
   if (!AllowTradesByTime())
   {
      CloseAllOrders();
      return;
   }
   
   if (!IsNewBar())
   {
      return;
   }
   
   CheckOrders();
   
   ////////////
 
   if (!IsBuyPositionOpened && UseBuy1)
   {
      int buy_high1_offset = iHighest(Symbol(), PERIOD_H1, MODE_HIGH, BuyOffset1, 0); 
      if(buy_high1_offset > BuyShift)
      {
         //recalculate first order
         double new_price = NormalizeDouble(iHigh(Symbol(), PERIOD_H1, buy_high1_offset) + Point * BuyExtremumOffset1, Digits);
         if (buyTicket1 != -1)
         {
            OrderSelect(buyTicket1, SELECT_BY_TICKET, MODE_TRADES);
            if (OrderOpenPrice() != new_price)
            {
               OrderDelete(buyTicket1);
               buyTicket1 = OrderSend(Symbol(), OP_BUYSTOP, BuyFixedLot1, new_price, Slippage,
                  NormalizeDouble(new_price - BuySL1 * Point, Digits), NormalizeDouble(new_price + BuyTP1 * Point, Digits), "", MagicNumber);
            }
         }
         else
         {
            buyTicket1 = OrderSend(Symbol(), OP_BUYSTOP, BuyFixedLot1, new_price,Slippage,
               NormalizeDouble(new_price - BuySL1 * Point, Digits), NormalizeDouble(new_price + BuyTP1 * Point, Digits), "", MagicNumber);
         }  
      }
      else
      {
         // delete first if exist
         if(buyTicket1 != -1)
         {
            OrderDelete(buyTicket1);
         }
      }
   }
    
    ////////////
   if (!IsSellPositionOpened && UseSell1)
   {
      int sell_low1_offset = iLowest(Symbol(), PERIOD_H1, MODE_LOW, SellOffset1, 0);
      if(sell_low1_offset > SellShift)
      {
         //recalculate first order
         double new_price = NormalizeDouble(iLow(Symbol(), PERIOD_H1 ,sell_low1_offset) - Point * SellExtremumOffset1, Digits);
         if (sellTicket1 != -1)
         {
            OrderSelect(sellTicket1, SELECT_BY_TICKET, MODE_TRADES);
            if (OrderOpenPrice() != new_price)
            {
               OrderDelete(sellTicket1);
               sellTicket1 = OrderSend(Symbol(), OP_SELLSTOP, SellFixedLot1, new_price, Slippage,
                  NormalizeDouble(new_price+SellSL1 * Point, Digits), NormalizeDouble(new_price - SellTP1 * Point, Digits), "", MagicNumber);
            }
         }
         else
         {
            sellTicket1 = OrderSend(Symbol(), OP_SELLSTOP, SellFixedLot1, new_price,Slippage,
               NormalizeDouble(new_price + SellSL1 * Point, Digits), NormalizeDouble(new_price - SellTP1 * Point, Digits), "", MagicNumber);
         }
      }
      else
      {
         // delete first if exist
         if (sellTicket1 != -1)
         {
            OrderDelete(sellTicket1);
         }
      }
   }
}
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{ 
}
//+------------------------------------------------------------------+
//| Tester function                                                  |
//+------------------------------------------------------------------+
double OnTester()
{
   double ret = 0.0;
   return(ret);
}
//+------------------------------------------------------------------+

void CheckOrders()
{
   for (int pos = 0; pos < OrdersTotal(); pos++)
   {
      if (!OrderSelect(pos, SELECT_BY_POS, MODE_TRADES))
         continue;
      if (OrderSymbol() != Symbol())
         continue;
      
      if (buyTicket1 == -1)
         if (OrderType() == OP_BUY ||OrderType()== OP_BUYLIMIT || OrderType()== OP_BUYSTOP)
             buyTicket1 = OrderTicket();
      if (sellTicket1 == -1)
         if (OrderType() == OP_SELL || OrderType() == OP_SELLLIMIT || OrderType() == OP_SELLSTOP)
             sellTicket1 = OrderTicket();
   }
   
   if (OrdersTotal() == 0)
   {
      buyTicket1 = -1;
      sellTicket1 = -1;
   }
   
   /*
   if (OrdersTotal() == 1)
   {
      OrderSelect(0, SELECT_BY_POS, MODE_TRADES);
      if (OrderType() == OP_BUY ||OrderType()== OP_BUYLIMIT || OrderType()== OP_BUYSTOP)
         sellTicket1 = -1;
      else    
     if (OrderType() == OP_SELL || OrderType() == OP_SELLLIMIT || OrderType() == OP_SELLSTOP)
         buyTicket1 = -1;   
   }*/
   
   if (buyTicket1 != -1)
   {
      if(OrderSelect(buyTicket1, SELECT_BY_TICKET, MODE_TRADES))
      {
         IsBuyPositionOpened = OrderType() == OP_BUY;
      }   
      else
      {
         buyTicket1 = -1;
         IsBuyPositionOpened = false;
      }
   }
   else
   {
      IsBuyPositionOpened = false;
   }
   
   if (sellTicket1 != -1)
   {
      if (OrderSelect(sellTicket1, SELECT_BY_TICKET, MODE_TRADES))
      {
         IsSellPositionOpened = OrderType() == OP_SELL;
      }    
      else
      {
         sellTicket1 = -1;
         IsSellPositionOpened = false;
      }  
   }
   else
   {
      IsSellPositionOpened = false;
   }
}

bool IsNewBar()
{
   datetime curBar = Time[0];
   if (lastBar != curBar)
   {
      lastBar = curBar;
      return true;
   }
   else
   {
      return false;
   }
}

void TrailingStops()
{
   for (int pos = 0; pos < OrdersTotal(); pos++)
   {
      if (!OrderSelect(pos, SELECT_BY_POS, MODE_TRADES))
         continue;
         
      //--- check for opened position and for symbol
      if ((OrderType() != OP_BUY && OrderType() != OP_SELL) || OrderSymbol() != Symbol() || OrderMagicNumber() != MagicNumber)
          continue;
          
      double buyThresholdPoints = NormalizeDouble(Point * BuyThreshold, Digits());
      double buyTrailingStopPoints = NormalizeDouble(Point * BuyTrailingStop, Digits());  
      double sellThresholdPoints = NormalizeDouble(Point * SellThreshold, Digits());
      double sellTrailingStopPoints = NormalizeDouble(Point * SellTrailingStop, Digits());   
      double priceDifference;
      double oldStopLoss = OrderStopLoss();    
      double newStopLoss;     
            
      //--- long position is opened
      if (OrderType() == OP_BUY)
      {
         priceDifference = NormalizeDouble((Bid - OrderOpenPrice()), Digits());
         newStopLoss = NormalizeDouble((Bid - buyTrailingStopPoints), Digits());   
         
         if (priceDifference < buyThresholdPoints)
         {   
            continue; 
         }     
         
         if (!IsStopLossPositive())
         {
            double priceShiftPoints = NormalizeDouble(Point * PriceShift, Digits());
            newStopLoss = NormalizeDouble(OrderOpenPrice() + priceShiftPoints, Digits());
            //Print("Threshold Passed!! (", OrderTicket(), ") OrderOpenPrice = ", OrderOpenPrice(), " Bid = ", Bid); 
         }

         if (oldStopLoss >= newStopLoss)
            continue;            
      }
      else //--- go to short position
      {
         priceDifference = NormalizeDouble((OrderOpenPrice() - Ask), Digits());
         newStopLoss = NormalizeDouble((Ask + sellTrailingStopPoints), Digits());  
         
         if (priceDifference < sellThresholdPoints)
         {  
            continue;
         }
         
         if (!IsStopLossPositive())
         {
            double priceShiftPoints = NormalizeDouble(Point * PriceShift, Digits());
            newStopLoss = NormalizeDouble(OrderOpenPrice() - priceShiftPoints, Digits()); 
            //Print("Threshold Passed!! (", OrderTicket(), ") OrderOpenPrice = ", OrderOpenPrice(), " Ask = ", Ask); 
         }

         if (oldStopLoss <= newStopLoss)
            continue; 
      }
      
      double currentPrice;
      if (OrderType() == OP_BUY)
         currentPrice = Bid;
      else
         currentPrice = Ask;
      
      //--- modify order
      if (!OrderModify(OrderTicket(), OrderOpenPrice(), newStopLoss, OrderTakeProfit(), 0))
         Print("OrderModify error: ", GetLastError(), " order (", OrderTicket(), ") OrderOpenPrice = ", OrderOpenPrice(), " old OrderStopLoss = ", oldStopLoss, " new OrderStopLoss = ", newStopLoss); 
      //else
      //   Print("OrderModify (", OrderTicket(), ") OrderOpenPrice = ", OrderOpenPrice(), " CurrentPrice = ", currentPrice, " old OrderStopLoss = ", oldStopLoss, " new OrderStopLoss = ", newStopLoss, " OrderComment = ", OrderComment()); 
   }
}

bool IsStopLossPositive()
{
   if (OrderType() == OP_BUY)
      return OrderStopLoss() > OrderOpenPrice();
   else
      return OrderStopLoss() < OrderOpenPrice();
}

bool AllowTradesByTime()
{
   int currentHour = TimeHour(TimeCurrent());

   if (currentHour < StartTraingHour || currentHour > EndTradingHour)
      return false;
      
   return true;
}

void CloseAllOrders()
{
   for (int pos = 0; pos < OrdersTotal(); pos++)
   {
      if (!OrderSelect(pos, SELECT_BY_POS, MODE_TRADES))
         continue;
      if (OrderSymbol() != Symbol())
         continue;    
      if ( OrderType()== OP_BUYSTOP)
      {
      OrderDelete(OrderTicket());
      }
   }
}
