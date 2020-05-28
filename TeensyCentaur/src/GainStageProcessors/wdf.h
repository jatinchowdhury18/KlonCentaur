#ifndef WDF_H_INCLUDED
#define WDF_H_INCLUDED

#include "omega.h"
#include <string>
#include <cmath>

namespace WaveDigitalFilter
{
/** Wave digital filter base class */
class WDF
{
public:
    WDF (std::string type) : type (type) {}
    virtual ~WDF() {}

    virtual inline void calcImpedance() {}
    virtual inline void propagateImpedance() {}

    virtual inline void incident (double x) noexcept {}
    virtual inline double reflected() noexcept { return 0.0; }


    inline double voltage() const noexcept
    {
        return (a + b) / 2.0;
    }

    inline double current() const noexcept
    {
        return (a - b) / (2.0 * R);
    }

public:
    double a = 0.0; // incident wave
    double b = 0.0; // reflected wave
    double R = 1.0e-9;
    double G = 1.0 / R;

private:
    const std::string type;
};

/** WDF node base class */
class WDFNode : public WDF
{
public:
    WDFNode (std::string type) : WDF (type) {}
    virtual ~WDFNode() {}

    void connectToNode (WDF* node)
    {
        next = node;
    }

    inline void propagateImpedance() override
    {
        calcImpedance();

        if (next != nullptr)
            next->propagateImpedance();
    }

protected:
    WDF* next = nullptr;
};

/** WDF Resistor Node */
class Resistor : public WDFNode
{
public:
    Resistor (double value) :
        WDFNode ("Resistor"),
        R_value (value)
    {
        calcImpedance();
    }
    virtual ~Resistor() {}

    void setResistanceValue (double newR)
    {
        if (newR == R_value)
            return;

        R_value = newR;
        propagateImpedance();
    }

    inline void calcImpedance() override
    {
        R = R_value;
        G = 1.0 / R;
    }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = 0.0;
        return b;
    }

private:
    double R_value = 1.0e-9;
};

/** WDF Capacitor Node */
class Capacitor : public WDFNode
{
public:
    Capacitor (double value, double fs, double alpha = 1.0) :
        WDFNode ("Capacitor"),
        C_value (value),
        fs (fs),
        alpha (alpha),
        b_coef ((1.0 - alpha) / 2.0),
        a_coef ((1.0 + alpha) / 2.0)
    {
        calcImpedance();
    }
    virtual ~Capacitor() {}

    void setCapacitanceValue (double newC)
    {
        if (newC == C_value)
            return;

        C_value = newC;
        propagateImpedance();
    }

    inline void calcImpedance() override
    {
        R = 1.0 / ((1.0 + alpha) * C_value * fs);
        G = 1.0 / R;
    }

    inline void incident (double x) noexcept override
    {
        a = x;
        z = a;
    }

    inline double reflected() noexcept override
    {
        b = b_coef * b + a_coef * z;
        return b;
    }

private:
    double C_value = 1.0e-6;
    double z = 0.0;

    const double fs;
    const double alpha;

    const double b_coef;
    const double a_coef;
};


/** WDF Inductor Node */
class Inductor : public WDFNode
{
public:
    Inductor (double value, double fs, double alpha = 1.0) :
        WDFNode ("Inductor"),
        L_value (value),
        fs (fs),
        alpha (alpha),
        b_coef ((1.0 - alpha) / 2.0),
        a_coef ((1.0 + alpha) / 2.0)
    {
        calcImpedance();
    }
    virtual ~Inductor() {}

    void setInductanceValue (double newL)
    {
        if (newL == L_value)
            return;

        L_value = newL;
        propagateImpedance();
    }

    inline void calcImpedance() override
    {
        R = (1.0 + alpha) * L_value * fs;
        G = 1.0 / R;
    }

    inline void incident (double x) noexcept override
    {
        a = x;
        z = a;
    }

    inline double reflected() noexcept override
    {
        b = b_coef * b - a_coef * z;
        return b;
    }

private:
    double L_value = 1.0e-6;
    double z = 0.0;

    const double fs;
    const double alpha;

    const double b_coef;
    const double a_coef;
};

/** WDF Switch */
class Switch : public WDFNode
{
public:
    Switch():
        WDFNode ("Switch")
    {}
    virtual ~Switch() {}

    inline void calcImpedance() override {}

    void setClosed (bool shouldClose) { closed = shouldClose; }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = closed ? -a : a;
        return b;
    }

private:
    bool closed = true;
};

/** WDF Open */
class Open : public WDFNode
{
public:
    Open():
        WDFNode ("Open")
    {}
    virtual ~Open()
    {
        R = 1.0e15;
        G = 1.0 / R;
    }

    inline void calcImpedance() override {}

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = a;
        return b;
    }
};

/** WDF Short */
class Short : public WDFNode
{
public:
    Short():
        WDFNode ("Short")
    {}
    virtual ~Short()
    {
        R = 1.0e-15;
        G = 1.0 / R;
    }

    inline void calcImpedance() override {}

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = -a;
        return b;
    }
};

/** WDF Voltage Polarity Inverter */
class PolarityInverter : public WDFNode
{
public:
    PolarityInverter (WDFNode* port1) :
        WDFNode ("Polarity Inverter"),
        port1 (port1)
    {
        port1->connectToNode (this);
        calcImpedance();
    }
    virtual ~PolarityInverter() {}

    inline void calcImpedance() override
    {
        R = port1->R;
        G = 1.0 / R;
    }

    inline void incident (double x) noexcept override
    {
        a = x;
        port1->incident (-x);
    }

    inline double reflected() noexcept override
    {
        b = -port1->reflected();
        return b;
    }

private:
    WDFNode* port1;
};

/** WDF y-parameter 2-port (short circuit admittance) */
class YParameter : public WDFNode
{
public:
    YParameter (WDFNode* port1, double y11, double y12, double y21, double y22) :
        WDFNode ("YParameter"),
        port1 (port1)
    {
        y[0][0] = y11; y[0][1] = y12;
        y[1][0] = y21; y[1][1] = y22;

        port1->connectToNode (this);
        calcImpedance();
    }

    virtual ~YParameter() {}

    inline void calcImpedance() override
    {
        denominator = y[1][1] + port1->R * y[0][0] * y[1][1] - port1->R * y[0][1] * y[1][0];
        R = (port1->R * y[0][0] + 1.0) / denominator;
        G = 1.0 / R;

        double rSq = port1->R * port1->R;
        double num1A = -y[1][1] * rSq * y[0][0] * y[0][0];
        double num2A = y[0][1] * y[1][0] * rSq * y[0][0];

        A = (num1A + num2A + y[1][1]) / (denominator * (port1->R * y[0][0] + 1.0));
        B = -port1->R * y[0][1] / (port1->R * y[0][0] + 1.0);
        C = -y[1][0] / denominator;
    }

    inline void incident (double x) noexcept override
    {
        a = x;
        port1->incident(A * port1->b + B * x);
    }

    inline double reflected() noexcept override
    {
        b = C * port1->reflected();
        return b;
    }

private:
    WDFNode* port1;
    double y[2][2] = {{ 0.0, 0.0 }, { 0.0, 0.0 }};
    
    double denominator = 1.0;
    double A = 1.0f;
    double B = 1.0f;
    double C = 1.0f;
};

/** WDF 3-port adapter base class */
class WDFAdaptor : public WDFNode
{
public:
    WDFAdaptor (WDFNode* port1, WDFNode* port2, std::string type) :
        WDFNode (type),
        port1 (port1),
        port2 (port2)
    {
        port1->connectToNode (this);
        port2->connectToNode (this);
    }
    virtual ~WDFAdaptor() {}

protected:
    WDFNode* port1;
    WDFNode* port2;
};

/** WDF 3-port parallel adaptor */
class WDFParallel : public WDFAdaptor
{
public:
    WDFParallel (WDFNode* port1, WDFNode* port2) :
        WDFAdaptor (port1, port2, "Parallel")
    {
        calcImpedance();
    }
    virtual ~WDFParallel() {}

    inline void calcImpedance()
    {
        G = port1->G + port2->G;
        R = 1.0 / G;
        port1Reflect = port1->G/G;
        port2Reflect = port2->G/G;
    }

    inline double reflected() noexcept override
    {
        b = port1Reflect * port1->reflected() + port2Reflect * port2->reflected();
        return b;
    }

    inline void incident (double x) noexcept override
    {
        port1->incident (x + (port2->b - port1->b) * port2Reflect);
        port2->incident (x + (port2->b - port1->b) * -port1Reflect);
        a = x;
    }

private:
    double port1Reflect = 1.0;
    double port2Reflect = 1.0;
};

/** WDF 3-port series adaptor */
class WDFSeries : public WDFAdaptor
{
public:
    WDFSeries (WDFNode* port1, WDFNode* port2) :
        WDFAdaptor (port1, port2, "Series")
    {
        calcImpedance();
    }
    virtual ~WDFSeries() {}

    inline void calcImpedance()
    {
        R = port1->R + port2->R;
        G = 1.0 / R;
        port1Reflect = port1->R/R;
        port2Reflect = port2->R/R;
    }

    inline double reflected() noexcept override
    {
        b = -(port1->reflected() + port2->reflected());
        return b;
    }

    inline void incident (double x) noexcept override
    {
        port1->incident (port1->b - port1Reflect * (x + port1->b + port2->b));
        port2->incident (port2->b - port2Reflect * (x + port1->b + port2->b)); 

        a = x;
    }

private:
    double port1Reflect = 1.0;
    double port2Reflect = 1.0;
};

/** WDF Voltage source with resistance */
class ResistiveVoltageSource : public WDFNode
{
public:
    ResistiveVoltageSource (double value = 1.0e-9) :
        WDFNode ("Resistive Voltage"),
        R_value (value)
    {
        calcImpedance();
    }
    virtual ~ResistiveVoltageSource() {}

    void setResistanceValue (double newR)
    {
        if (newR == R_value)
            return;

        R_value = newR;
        propagateImpedance();
    }

    inline void calcImpedance()
    {
        R = R_value;
        G = 1.0 / R;
    }

    void setVoltage (double newV) { Vs = newV; }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = Vs;
        return b;
    }

private:
    double Vs;
    double R_value = 1.0e-9;
};

/** WDF Voltage source with 1 pOhm resistance */
class IdealVoltageSource : public WDFNode
{
public:
    IdealVoltageSource() : WDFNode ("IdealVoltage")
    {
        calcImpedance();
    }
    virtual ~IdealVoltageSource() {}

    inline void calcImpedance() {}

    void setVoltage (double newV) { Vs = newV; }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = -a + 2.0 * Vs;
        return b;
    }

private:
    double Vs;
};

/** WDF Current source with resistance */
class ResistiveCurrentSource : public WDFNode
{
public:
    ResistiveCurrentSource (double value=1.0e9) :
        WDFNode ("Resistive Current"),
        R_value (value)
    {
        calcImpedance();
    }
    virtual ~ResistiveCurrentSource() {}

    void setResistanceValue (double newR)
    {
        if (newR == R_value)
            return;

        R_value = newR;
        propagateImpedance();
    }

    inline void calcImpedance()
    {
        R = R_value;
        G = 1.0 / R;
    }

    void setCurrent (double newI) { Is = newI; }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = 2 * R * Is;
        return b;
    }

private:
    double Is;
    double R_value = 1.0e9;
};

/** WDF Current source with 1 GOhm resistance */
class IdealCurrentSource : public WDFNode
{
public:
    IdealCurrentSource() : WDFNode ("Ideal Current")
    {
        calcImpedance();
    }
    virtual ~IdealCurrentSource() {}

    inline void calcImpedance()
    {
        R = 1.0e9;
        G = 1.0 / R;
    }

    void setCurrent (double newI) { Is = newI; }

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        b = 2 * next->R * Is + a;
        return b;
    }

private:
    double Is;
};

template <typename T> inline int signum (T val)
{
    return (T (0) < val) - (val < T (0));
}

/**
 * Diode pair nonlinearity evaluated in the wave domain
 * See Werner et al., "An Improved and Generalized Diode Clipper Model for Wave Digital Filters"
 * https://www.researchgate.net/publication/299514713_An_Improved_and_Generalized_Diode_Clipper_Model_for_Wave_Digital_Filters
 * */
class DiodePair : public WDFNode
{
public:
    DiodePair (double Is, double Vt) :
        WDFNode ("DiodePair"),
        Is (Is),
        Vt (Vt)
    {}

    virtual ~DiodePair() {}

    inline void calcImpedance() {}

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        // See eqn (18) from reference paper
        double lambda = (double) signum (a);
        b = a + 2 * lambda * (next->R * Is - Vt * omega4 (float (log (next->R * Is / Vt) + (lambda * a + next->R * Is) / Vt)));
        return b;
    }

private:
    const double Is; // reverse saturation current
    const double Vt; // thermal voltage
};

class Diode : public WDFNode
{
public:
    Diode (double Is, double Vt) :
        WDFNode ("Diode"),
        Is (Is),
        Vt (Vt)
    {}

    virtual ~Diode() {}

    inline void calcImpedance() {}

    inline void incident (double x) noexcept override
    {
        a = x;
    }

    inline double reflected() noexcept override
    {
        // See eqn (10) from reference paper
        b = a + 2 * next->R * Is - 2 * Vt * omega4 (float (log (next->R * Is / Vt) + (a + next->R * Is) / Vt));
        return b;
    }

private:
    const double Is; // reverse saturation current
    const double Vt; // thermal voltage
};

}

#endif // WDF_H_INCLUDED
