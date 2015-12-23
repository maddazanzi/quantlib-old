/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006 Eric Ehlers
 Copyright (C) 2006, 2011, 2015 Ferdinando Ametrano
 Copyright (C) 2005 Aurelien Chanudet
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2006 Katiuscia Manzoni
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

#if defined(HAVE_CONFIG_H)
    #include <qlo/config.hpp>
#endif

#include <qlo/basisswap.hpp>

using std::vector;
using boost::shared_ptr;
using ObjectHandler::ValueObject;
using ObjectHandler::property_t;

namespace QuantLibAddin {

    BasisSwap::BasisSwap(const shared_ptr<ValueObject>& properties,
                             QuantLib::BasisSwap::Type type,
                             QuantLib::Real nominal,
                             const boost::shared_ptr<QuantLib::Schedule>& shortTenorSchedule,
                             const boost::shared_ptr<QuantLib::IborIndex>& shortTenorIndex,
                             const QuantLib::DayCounter& shortTenorDayCount,
                             QuantLib::Spread shortTenorSpread,
                             const boost::shared_ptr<QuantLib::Schedule>& longTenorSchedule,
                             const boost::shared_ptr<QuantLib::IborIndex>& longTenorIndex,
                             const QuantLib::DayCounter& longTenorDayCount,
                             QuantLib::Spread longTenorSpread,
                             QuantLib::BusinessDayConvention paymentConvention,
                             bool permanent)
    : Swap(properties, permanent)
    {
        libraryObject_ = shared_ptr<QuantLib::Instrument>(new
            QuantLib::BasisSwap(type,
                                nominal,
                                *shortTenorSchedule,
                                shortTenorIndex,
                                shortTenorDayCount,
                                shortTenorSpread,
                                *longTenorSchedule,
                                longTenorIndex,
                                longTenorDayCount,
                                longTenorSpread,
                                paymentConvention));
    }

}
