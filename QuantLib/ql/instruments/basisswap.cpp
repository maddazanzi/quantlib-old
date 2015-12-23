/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2007 Ferdinando Ametrano
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

#include <ql/instruments/basisswap.hpp>
#include <ql/cashflows/iborcoupon.hpp>
#include <ql/cashflows/cashflowvectors.hpp>
#include <ql/cashflows/cashflows.hpp>
#include <ql/cashflows/couponpricer.hpp>
#include <ql/indexes/iborindex.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>

namespace QuantLib {

    BasisSwap::BasisSwap(
                     Type type,
                     Real nominal,
                     const Schedule& shortTenorSchedule,
                     const boost::shared_ptr<IborIndex>& shortTenorIndex,
                     const DayCounter& shortTenorDayCount,
                     Spread shortTenorSpread,
                     const Schedule& longTenorSchedule,
                     const boost::shared_ptr<IborIndex>& longTenorIndex,
                     const DayCounter& longTenorDayCount,
                     Spread longTenorSpread,
                     boost::optional<BusinessDayConvention> paymentConvention)
    : Swap(2), type_(type), nominal_(nominal),
    shortTenorSchedule_(shortTenorSchedule), shortTenorIndex_(shortTenorIndex),
    shortTenorDayCount_(shortTenorDayCount), shortTenorSpread_(shortTenorSpread),
    longTenorSchedule_(longTenorSchedule), longTenorIndex_(longTenorIndex),
    longTenorDayCount_(longTenorDayCount), longTenorSpread_(longTenorSpread) {

        if (paymentConvention)
            paymentConvention_ = *paymentConvention;
        else
            paymentConvention_ = shortTenorSchedule_.businessDayConvention();

        legs_[0] = IborLeg(shortTenorSchedule_, shortTenorIndex_)
            .withNotionals(nominal_)
            .withPaymentDayCounter(shortTenorDayCount_)
            .withPaymentAdjustment(paymentConvention_)
            .withSpreads(shortTenorSpread_);

        legs_[1] = IborLeg(longTenorSchedule_, longTenorIndex_)
            .withNotionals(nominal_)
            .withPaymentDayCounter(longTenorDayCount_)
            .withPaymentAdjustment(paymentConvention_)
            .withSpreads(longTenorSpread_);

        for (Leg::const_iterator i = legs_[1].begin(); i < legs_[1].end(); ++i)
            registerWith(*i);

        switch (type_) {
          case Payer:
            payer_[0] = -1.0;
            payer_[1] = +1.0;
            break;
          case Receiver:
            payer_[0] = +1.0;
            payer_[1] = -1.0;
            break;
          default:
            QL_FAIL("Unknown basis-swap type");
        }
    }

    void BasisSwap::setupArguments(PricingEngine::arguments* args) const {

        Swap::setupArguments(args);

        BasisSwap::arguments* arguments =
            dynamic_cast<BasisSwap::arguments*>(args);

        if (!arguments)  // it's a swap engine...
            return;

        arguments->type = type_;
        arguments->nominal = nominal_;

        const Leg& shortTenorCoupons = shortTenorLeg();

        arguments->shortTenorResetDates = arguments->shortTenorPayDates =
            arguments->shortTenorFixingDates =
            std::vector<Date>(shortTenorCoupons.size());
        arguments->shortTenorAccrualTimes =
            std::vector<Time>(shortTenorCoupons.size());
        arguments->shortTenorSpreads =
            std::vector<Spread>(shortTenorCoupons.size());
        arguments->shortTenorCoupons = std::vector<Real>(shortTenorCoupons.size());
        for (Size i=0; i<shortTenorCoupons.size(); ++i) {
            boost::shared_ptr<IborCoupon> coupon =
                boost::dynamic_pointer_cast<IborCoupon>(shortTenorCoupons[i]);

            arguments->shortTenorResetDates[i] = coupon->accrualStartDate();
            arguments->shortTenorPayDates[i] = coupon->date();
            arguments->shortTenorFixingDates[i] = coupon->fixingDate();
            arguments->shortTenorAccrualTimes[i] = coupon->accrualPeriod();
            arguments->shortTenorSpreads[i] = coupon->spread();
            try {
                arguments->shortTenorCoupons[i] = coupon->amount();
            } catch (Error&) {
                arguments->shortTenorCoupons[i] = Null<Real>();
            }
        }

        const Leg& longTenorCoupons = longTenorLeg();

        arguments->longTenorResetDates = arguments->longTenorPayDates =
            arguments->longTenorFixingDates =
            std::vector<Date>(longTenorCoupons.size());
        arguments->longTenorAccrualTimes =
            std::vector<Time>(longTenorCoupons.size());
        arguments->longTenorSpreads =
            std::vector<Spread>(longTenorCoupons.size());
        arguments->longTenorCoupons = std::vector<Real>(longTenorCoupons.size());
        for (Size i = 0; i<longTenorCoupons.size(); ++i) {
            boost::shared_ptr<IborCoupon> coupon =
                boost::dynamic_pointer_cast<IborCoupon>(longTenorCoupons[i]);

            arguments->longTenorResetDates[i] = coupon->accrualStartDate();
            arguments->longTenorPayDates[i] = coupon->date();
            arguments->longTenorFixingDates[i] = coupon->fixingDate();
            arguments->longTenorAccrualTimes[i] = coupon->accrualPeriod();
            arguments->longTenorSpreads[i] = coupon->spread();
            try {
                arguments->longTenorCoupons[i] = coupon->amount();
            }
            catch (Error&) {
                arguments->longTenorCoupons[i] = Null<Real>();
            }
        }
    }

    Spread BasisSwap::shortTenorLegFairSpread() const {
        calculate();
        QL_REQUIRE(shortTenorLegFairSpread_ != Null<Spread>(), "result not available");
        return shortTenorLegFairSpread_;
    }

    Spread BasisSwap::longTenorLegFairSpread() const {
        calculate();
        QL_REQUIRE(longTenorLegFairSpread_ != Null<Spread>(), "result not available");
        return longTenorLegFairSpread_;
    }

    Real BasisSwap::shortTenorLegBPS() const {
        calculate();
        QL_REQUIRE(legBPS_[0] != Null<Real>(), "result not available");
        return legBPS_[0];
    }

    Real BasisSwap::longTenorLegBPS() const {
        calculate();
        QL_REQUIRE(legBPS_[1] != Null<Real>(), "result not available");
        return legBPS_[1];
    }

    Real BasisSwap::shortTenorLegNPV() const {
        calculate();
        QL_REQUIRE(legNPV_[0] != Null<Real>(), "result not available");
        return legNPV_[0];
    }

    Real BasisSwap::longTenorLegNPV() const {
        calculate();
        QL_REQUIRE(legNPV_[1] != Null<Real>(), "result not available");
        return legNPV_[1];
    }

    void BasisSwap::setupExpired() const {
        Swap::setupExpired();
        legBPS_[0] = legBPS_[1] = 0.0;
        shortTenorLegFairSpread_ = Null<Spread>();
        longTenorLegFairSpread_ = Null<Spread>();
    }

    void BasisSwap::fetchResults(const PricingEngine::results* r) const {
        static const Spread basisPoint = 1.0e-4;

        Swap::fetchResults(r);

        const BasisSwap::results* results =
            dynamic_cast<const BasisSwap::results*>(r);
        if (results) { // might be a swap engine, so no error is thrown
            shortTenorLegFairSpread_ = results->shortTenorLegFairSpread;
            longTenorLegFairSpread_ = results->longTenorLegFairSpread;
        } else {
            shortTenorLegFairSpread_ = Null<Spread>();
            longTenorLegFairSpread_ = Null<Spread>();
        }

        if (shortTenorLegFairSpread_ == Null<Spread>()) {
            // calculate it from other results
            if (legBPS_[0] != Null<Real>())
                shortTenorLegFairSpread_ = shortTenorSpread_ - NPV_ / (legBPS_[0] / basisPoint);
        }
        if (longTenorLegFairSpread_ == Null<Spread>()) {
            // ditto
            if (legBPS_[1] != Null<Real>())
                longTenorLegFairSpread_ = longTenorSpread_ - NPV_ / (legBPS_[1] / basisPoint);
        }
    }

    void BasisSwap::arguments::validate() const {
        Swap::arguments::validate();
        QL_REQUIRE(nominal != Null<Real>(), "nominal null or not set");

        QL_REQUIRE(shortTenorResetDates.size() == shortTenorPayDates.size(),
                   "number of shorter tenor leg start dates different from "
                   "number of shorter tenor leg payment dates");
        QL_REQUIRE(shortTenorFixingDates.size() == shortTenorPayDates.size(),
                   "number of shorter tenor leg fixing dates different from "
                   "number of shorter tenor leg payment dates");
        QL_REQUIRE(shortTenorAccrualTimes.size() == shortTenorPayDates.size(),
                   "number of shorter tenor leg accrual Times different from "
                   "number of shorter tenor leg payment dates");
        QL_REQUIRE(shortTenorSpreads.size() == shortTenorPayDates.size(),
                   "number of shorter tenor leg spreads different from "
                   "number of shorter tenor leg payment dates");
        QL_REQUIRE(shortTenorPayDates.size() == shortTenorCoupons.size(),
                   "number of shorter tenor leg payment dates different from "
                   "number of shorter tenor leg coupon amounts");

        QL_REQUIRE(longTenorResetDates.size() == longTenorPayDates.size(),
            "number of longer tenor leg start dates different from "
            "number of longer tenor leg payment dates");
        QL_REQUIRE(longTenorFixingDates.size() == longTenorPayDates.size(),
            "number of longer tenor leg fixing dates different from "
            "number of longer tenor leg payment dates");
        QL_REQUIRE(longTenorAccrualTimes.size() == longTenorPayDates.size(),
            "number of longer tenor leg accrual Times different from "
            "number of longer tenor leg payment dates");
        QL_REQUIRE(longTenorSpreads.size() == longTenorPayDates.size(),
            "number of longer tenor leg spreads different from "
            "number of longer tenor leg payment dates");
        QL_REQUIRE(longTenorPayDates.size() == longTenorCoupons.size(),
            "number of longer tenor leg payment dates different from "
            "number of longer tenor leg coupon amounts");
    }

    void BasisSwap::results::reset() {
        Swap::results::reset();
        shortTenorLegFairSpread = Null<Spread>();
        longTenorLegFairSpread = Null<Spread>();
    }

    std::ostream& operator<<(std::ostream& out,
                             BasisSwap::Type t) {
        switch (t) {
          case BasisSwap::Payer:
            return out << "Payer";
          case BasisSwap::Receiver:
            return out << "Receiver";
          default:
            QL_FAIL("unknown BasisSwap::Type(" << Integer(t) << ")");
        }
    }

}
