/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000-2004 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file cashflowvectors.hpp
    \brief Cash flow vector builders
*/

#ifndef quantlib_cash_flow_vectors_hpp
#define quantlib_cash_flow_vectors_hpp

#include <ql/cashflow.hpp>
#include <ql/Indexes/xibor.hpp>
#include <ql/schedule.hpp>

namespace QuantLib {

    //! helper function building a sequence of fixed rate coupons
    std::vector<boost::shared_ptr<CashFlow> > FixedRateCouponVector(
        const Schedule& schedule,
        BusinessDayConvention paymentAdjustment,
        const std::vector<Real>& nominals,
        const std::vector<Rate>& couponRates,
        const DayCounter& dayCount,
        const DayCounter& firstPeriodDayCount = DayCounter());

    //! helper function building a sequence of par coupons
    /*! Either UpFrontIndexedCoupons or ParCoupons are used depending
        on the library configuration.

        \todo A suitable algorithm should be implemented for the
              calculation of the interpolated index fixing for a
              short/long first coupon.
    */
    std::vector<boost::shared_ptr<CashFlow> > FloatingRateCouponVector(
        const Schedule& schedule,
        const BusinessDayConvention paymentAdjustment,
        const std::vector<Real>& nominals,
        const Integer fixingDays,
        const boost::shared_ptr<Xibor>& index,
        const std::vector<Real>& gearings
        #if !defined(QL_PATCH_MSVC6)
        = std::vector<Real>()
        #endif
        ,
        const std::vector<Spread>& spreads
        #if !defined(QL_PATCH_MSVC6)
        = std::vector<Spread>()
        #endif
        ,
        const DayCounter& dayCounter = DayCounter());

    #ifndef QL_DISABLE_DEPRECATED
    //! \deprecated use the gearing-enabled function instead
    inline std::vector<boost::shared_ptr<CashFlow> > FloatingRateCouponVector(
        const Schedule& schedule,
        BusinessDayConvention paymentAdjustment,
        const std::vector<Real>& nominals,
        const boost::shared_ptr<Xibor>& index,
        Integer fixingDays,
        const std::vector<Spread>& spreads
        #if !defined(QL_PATCH_MSVC6)
        = std::vector<Spread>()
        #endif
        ,
        const DayCounter& dayCounter = DayCounter())
    {
        return FloatingRateCouponVector(schedule, paymentAdjustment, nominals,
                                        fixingDays, index,
                                        std::vector<Real>(1, 1.0), spreads,
                                        dayCounter);
    }
    #endif

}

#endif
