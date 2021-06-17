#ifndef DIODEPAIR_H_INCLUDED
#define DIODEPAIR_H_INCLUDED

#include <pch.h>

namespace GainStageSpace
{

using namespace chowdsp::WDFT;

/** WDF Diode Pair based on chowdsp::WDFT::DiodePair,
 *  but with customisations for quiet signals.
 */
template <typename T, typename Next>
class CustomDiodePairT final : public RootWDF
{
public:
    /** Creates a new WDF diode pair, with the given diode specifications.
     * @param Is: reverse saturation current
     * @param Vt: thermal voltage
     * @param next: the next element in the WDF connection tree
     */
    CustomDiodePairT (T Is, T Vt, Next& n);

    inline void calcImpedance() override
    {
        R_Is = next.R * Is;
        R_Is_overVt = R_Is * oneOverVt;
        logR_Is_overVt = std::log (R_Is_overVt);
    }

    /** Accepts an incident wave into a WDF diode pair. */
    inline void incident (T x) noexcept
    {
        a = x;
    }

    /** Propogates a reflected wave from a WDF diode pair. */
    inline T reflected() noexcept
    {
        // See eqn (18) from reference paper
        T lambda = (T) chowdsp::signum (a);
        T wrightIn = logR_Is_overVt + lambda * a * oneOverVt + R_Is_overVt;

        // Stefano D'Angelo's Wright Omega function is good at most values,
        // but has errors near zero, which cause audible distortion on very
        // quiet signal. So for quiet signals we use an LUT.
        if (std::abs (wrightIn) > 0.5)
        {
            b = a + 2 * lambda * (R_Is - Vt * chowdsp::Omega::omega4 (wrightIn));
        }
        else
        {
            b = a + 2 * lambda * (R_Is - Vt * wrightOmegaLUT.processSampleUnchecked (wrightIn));
        }

        return b;
    }

    T a = (T) 0.0; /* incident wave */
    T b = (T) 0.0; /* reflected wave */

private:
    const T Is; // reverse saturation current
    const T Vt; // thermal voltage

    // pre-computed vars
    const T oneOverVt;
    T R_Is;
    T R_Is_overVt;
    T logR_Is_overVt;

    Next& next;

    // lookup table
    static dsp::LookupTableTransform<double> wrightOmegaLUT;
    static bool lutIsInitialised;
};

} // namespace GainStageSpace

#endif // DIODEPAIR_H_INCLUDED
