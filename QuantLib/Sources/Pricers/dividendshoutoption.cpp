
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.org/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, please email quantlib-users@lists.sourceforge.net
 * The license is also available at http://quantlib.org/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.org/group.html
*/

/*! \file dividendshoutoption.cpp

    \fullpath
    Sources/Pricers/%dividendshoutoption.cpp
    \brief base class for shout options with dividends

*/

// $Id$
// $Log$
// Revision 1.14  2001/08/31 15:23:47  sigmud
// refining fullpath entries for doxygen documentation
//
// Revision 1.13  2001/08/09 14:59:48  sigmud
// header modification
//
// Revision 1.12  2001/08/08 11:07:50  sigmud
// inserting \fullpath for doxygen
//
// Revision 1.11  2001/08/07 11:25:55  sigmud
// copyright header maintenance
//
// Revision 1.10  2001/08/06 15:43:34  nando
// BSMOption now is SingleAssetOption
// BSMEuropeanOption now is EuropeanOption
//
// Revision 1.9  2001/07/25 15:47:29  sigmud
// Change from quantlib.sourceforge.net to quantlib.org
//
// Revision 1.8  2001/05/24 15:40:10  nando
// smoothing #include xx.hpp and cutting old Log messages
//

#include "ql/Pricers/dividendshoutoption.hpp"

namespace QuantLib {

    namespace Pricers {

        DividendShoutOption::DividendShoutOption(Type type,
                double underlying, double strike, Rate dividendYield,
                Rate riskFreeRate, Time residualTime, double volatility,
                const std::vector<double>& dividends,
                const std::vector<Time>& exdivdates,
                int timeSteps, int gridPoints)
       : DividendOption(type, underlying, strike, dividendYield,
                        riskFreeRate, residualTime, volatility,
                        dividends, exdivdates, timeSteps, gridPoints){}

        void DividendShoutOption::initializeStepCondition() const {
            stepCondition_ = Handle<FiniteDifferences::StandardStepCondition>(
                             new ShoutCondition(initialPrices_, residualTime_,
                                                riskFreeRate_));
        }

        Handle<SingleAssetOption> DividendShoutOption::clone() const {
            return Handle<SingleAssetOption>(new DividendShoutOption(*this));
        }

    }

}
