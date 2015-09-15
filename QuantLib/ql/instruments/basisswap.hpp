/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2006, 2008 Ferdinando Ametrano
 Copyright (C) 2015 Maddalena Zanzi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file basisswap.hpp
    \brief Simple Libor vs Libor swap
*/

#ifndef quantlib_basis_swap_hpp
#define quantlib_basis_swap_hpp

#include <ql/instruments/swap.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/schedule.hpp>
#include <boost/optional.hpp>

namespace QuantLib {

    class IborIndex;

    //! Simple basis swap: both legs are floating legs and depend on an Ibor index 
    //in the same currency.In most cases, the indexes have different tenors.
    //A spread above the Ibor index is paid on one of the legs.The quoting convention 
    //is to quote the spread on the shorter tenor leg, in such a way that the spread
    //is positive (the spread on the shorter tenor leg is set to 0.0 by default).

    /*! \ingroup instruments

        If no payment convention is passed, the convention of the
        floating-rate schedule is used.

        \warning if <tt>Settings::includeReferenceDateCashFlows()</tt>
                 is set to <tt>true</tt>, payments occurring at the
                 settlement date of the swap might be included in the
                 NPV and therefore affect the
                 fair-spreads calculation. This might not be what you
                 want.

        \test

    */
    class BasisSwap : public Swap {
      public:
        enum Type { Receiver = -1, Payer = 1 };
        class arguments;
        class results;
        class engine;
        BasisSwap(
            Type type,
            Real nominal,
            const Schedule& shortTenorSchedule,
            const boost::shared_ptr<IborIndex>& shortTenorIndex,
            const DayCounter& shortTenorDayCount,
            Spread shortTenorSpread,
            const Schedule& longTenorSchedule,
            const boost::shared_ptr<IborIndex>& longTenorIndex,
            const DayCounter& longTenorDayCount,
            Spread longTenorSpread = 0.0,
            boost::optional<BusinessDayConvention> paymentConvention =
                                                                 boost::none);
        //! \name Inspectors
        //@{
        Type type() const;
        Real nominal() const;

        const Schedule& shortTenorSchedule() const;
        const boost::shared_ptr<IborIndex>& shortTenorIndex() const;
        const DayCounter& shortTenorDayCount() const;
        Spread shortTenorSpread() const;

        const Schedule& longTenorSchedule() const;
        const boost::shared_ptr<IborIndex>& longTenorIndex() const;
        const DayCounter& longTenorDayCount() const;
        Spread longTenorSpread() const;

        BusinessDayConvention paymentConvention() const;

        const Leg& shortTenorLeg() const;
        const Leg& longTenorLeg() const;
        //@}

        //! \name Results
        //@{
        Real shortTenorLegBPS() const;
        Real shortTenorLegNPV() const;
        Spread shortTenorLegFairSpread() const;

        Real longTenorLegBPS() const;
        Real longTenorLegNPV() const;
        Rate longTenorLegFairSpread() const;
        //@}
        // other
        void setupArguments(PricingEngine::arguments* args) const;
        void fetchResults(const PricingEngine::results*) const;
      private:
        void setupExpired() const;

        Type type_;
        Real nominal_;

        Schedule shortTenorSchedule_;
        boost::shared_ptr<IborIndex> shortTenorIndex_;
        DayCounter shortTenorDayCount_;
        Spread shortTenorSpread_;

        Schedule longTenorSchedule_;
        boost::shared_ptr<IborIndex> longTenorIndex_;
        DayCounter longTenorDayCount_;
        Spread longTenorSpread_;

        BusinessDayConvention paymentConvention_;
        // results
        mutable Spread shortTenorLegFairSpread_;
        mutable Spread longTenorLegFairSpread_;
    };

    //! %Arguments for simple swap calculation

    class BasisSwap::arguments : public Swap::arguments {
      public:
        arguments() : type(Receiver),
                      nominal(Null<Real>()) {}
        Type type;
        Real nominal;

        std::vector<Time> shortTenorAccrualTimes;
        std::vector<Date> shortTenorResetDates;
        std::vector<Date> shortTenorFixingDates;
        std::vector<Date> shortTenorPayDates;

        std::vector<Time> longTenorAccrualTimes;
        std::vector<Date> longTenorResetDates;
        std::vector<Date> longTenorFixingDates;
        std::vector<Date> longTenorPayDates;

        std::vector<Spread> shortTenorSpreads;
        std::vector<Real> shortTenorCoupons;

        std::vector<Spread> longTenorSpreads;
        std::vector<Real> longTenorCoupons;
        void validate() const;
    };

    //! %Results from simple swap calculation
    class BasisSwap::results : public Swap::results {
      public:
        Spread shortTenorLegFairSpread;
        Spread longTenorLegFairSpread;
        void reset();
    };

    class BasisSwap::engine : public GenericEngine<BasisSwap::arguments,
                                                     BasisSwap::results> {};


    // inline definitions

    inline BasisSwap::Type BasisSwap::type() const {
        return type_;
    }

    inline Real BasisSwap::nominal() const {
        return nominal_;
    }

    inline const Schedule& BasisSwap::shortTenorSchedule() const {
        return shortTenorSchedule_;
    }

    inline const boost::shared_ptr<IborIndex>& BasisSwap::shortTenorIndex() const {
        return shortTenorIndex_;
    }

    inline Spread BasisSwap::shortTenorSpread() const {
        return shortTenorSpread_;
    }

    inline const DayCounter& BasisSwap::shortTenorDayCount() const {
        return shortTenorDayCount_;
    }

    inline const Schedule& BasisSwap::longTenorSchedule() const {
        return longTenorSchedule_;
    }

    inline const boost::shared_ptr<IborIndex>& BasisSwap::longTenorIndex() const {
        return longTenorIndex_;
    }

    inline Spread BasisSwap::longTenorSpread() const {
        return longTenorSpread_;
    }

    inline const DayCounter& BasisSwap::longTenorDayCount() const {
        return longTenorDayCount_;
    }
    inline BusinessDayConvention BasisSwap::paymentConvention() const {
        return paymentConvention_;
    }

    inline const Leg& BasisSwap::shortTenorLeg() const {
        return legs_[0];
    }

    inline const Leg& BasisSwap::longTenorLeg() const {
        return legs_[1];
    }

    std::ostream& operator<<(std::ostream& out,
                             BasisSwap::Type t);

}

#endif
