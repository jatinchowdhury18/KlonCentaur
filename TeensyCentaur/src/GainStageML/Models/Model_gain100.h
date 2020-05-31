#ifndef MODELGAIN1008_H_INCLUDED
#define MODELGAIN1008_H_INCLUDED

#include "../GainStageML.h"

GainStageModel<8, float> ModelGain100 = {
    {{ // GRUKernelWeights
        -1.2139369f,   0.3385323f,   0.80420727f,  1.6083051f,   1.4925718f,   1.3737986f,
         0.7825978f,  -0.56714857f, -1.9341521f,   1.5931427f,  -1.6734984f,  -1.1063985f,
         1.7804472f,   0.67403615f,  0.89329886f, -0.51074517f, -0.23955683f,  0.7172214f,
        -1.0044461f,  -0.9335916f,   0.07492222f,  0.51499724f, -0.03956257f,  1.6917198f, 
    }},

    {{ // GRURecurrentWeights
         3.141804f,    0.67058885f,  2.6651661f,  -0.47483295f, -1.0735898f,  -0.86757445f,
        -0.1796108f,   1.6244127f,  -1.7627958f,  -0.21538673f, -0.68624073f, -0.16227622f,
        -1.9899952f,  -0.6517866f,   0.12845302f, -0.22712867f,  1.0972071f,   0.23319997f,
        -0.9037699f,   0.5206569f,   0.450195f,    0.2719004f,   0.02462878f,  0.04783935f, },
       {-4.12596f,    -0.37444943f, -2.7517006f,  -0.29510546f, -0.88339454f,  0.88163555f,
         1.5889136f,  -4.319661f,   -0.8705499f,   0.3835366f,   0.14798161f, -0.6847f,
         0.22013366f, -0.3148343f,   0.10226817f,  1.1126875f,   0.31557336f,  0.49231905f,
        -0.5117021f,   0.546042f,    0.6252459f,   0.3578866f,   0.4606673f,   0.34613645f, },
       {-1.8204486f,  -0.14154401f, -1.0374238f,  -0.12069672f,  1.4717772f,   1.7998067f,
         1.584553f,   -1.026908f,    1.319417f,    0.10831594f,  0.7346144f,   1.2672064f,
         1.863549f,   -0.44397247f, -0.31955013f, -0.60979056f, -0.58167547f, -0.33757186f,
         0.3226781f,  -0.05887755f, -0.18357728f, -0.3705915f,  -0.24613434f, -0.00704994f, },
       {-2.021561f,    0.49756446f, -0.30117846f, -0.44629768f,  0.40414488f,  1.8876151f,
         0.21780556f, -1.3006269f,   0.08414963f,  0.36508253f,  0.6312261f,   0.5022784f,
         0.7274996f,  -1.0849671f,   0.4348315f,   1.4218261f,  -0.76384425f,  0.10002236f,
         0.40649396f,  0.11637715f,  0.36665604f,  0.57892746f,  0.90598017f, -0.59815174f, },
       {-1.7647609f,  -0.01210297f,  0.89860624f, -0.97938013f, -0.5776816f,  -0.04211174f,
        -0.00433548f,  4.006452f,   -0.5038501f,   0.35810244f, -0.86057395f,  0.8300092f,
         0.8739552f,   0.4124341f,  -1.0430365f,  -0.84326804f,  0.3877162f,   0.5794478f,
         0.16629633f, -0.2231631f,   0.5476802f,  -0.4472531f,  -0.2681104f,  -0.3678967f,  },
       {-2.5809216f,  -1.3351521f,   0.10584637f, -0.57550067f,  0.9301505f,  -1.3555326f,
        -1.8939396f,   1.7893584f,   2.584669f,   -0.83844954f,  0.1726339f,   0.66023165f,
         0.265165f,    0.44711536f,  0.06520545f,  0.38311222f, -1.197697f,   -0.89168596f,
         1.4506246f,   0.7685483f,  -0.26355f,     1.0817158f,  -0.65116876f, -1.7876749f,  },
       { 2.6200466f,  -0.01892568f,  0.05581176f, -0.56824446f, -0.5439765f,   0.19408575f,
         0.21479277f, -0.328629f,   -0.45227778f, -0.5151282f,  -0.5981347f,   0.3245713f,
         0.31973675f, -0.1503376f,   0.22088172f,  1.394541f,   -0.2451104f,  -0.3729873f,
        -0.8970123f,   0.41759062f, -0.49992839f,  0.44834486f,  0.21642014f,  0.6118992f,  },
       { 4.1868796f,   1.6725602f,   1.2358257f,  -0.802163f,   -0.8140879f,  -0.67581f,
        -0.0947513f,   0.8632045f,  -2.5856109f,  -0.20327485f, -0.9125204f,  -0.4258088f,
        -0.9158329f,  -0.35309163f,  0.8767788f,  -0.39496553f,  1.2591833f,  -0.12940939f,
        -0.7328527f,   0.7637704f,   0.09033938f,  0.3005506f,  -0.40472215f,  0.6820012f,  }
    },

    {{ // GRUBias
        -1.0236852f,   0.58247596f,  0.00855568f, -0.34669265f,  0.42616755f,  0.86796373f,
         0.90644836f, -0.2593728f,  -0.14748247f,  0.11211689f,  0.65369177f,  0.98476696f,
        -0.03422693f,  0.20028086f,  0.10750532f,  0.69231653f,  0.29449123f,  0.05206126f,
         0.03069792f,  0.09513818f,  0.0919253f,  -0.00130296f, -0.10311135f,  0.09793771f, },
      { -0.97872025f,  0.55254847f,  0.07443211f, -0.2915647f,   0.36239183f,  0.9193149f,
         0.96163225f, -0.2559162f,  -0.10058709f,  0.14277606f,  0.64612556f,  1.0097448f,
        -0.09136689f,  0.11158562f,  0.0760247f,   0.70268923f, -0.31678623f,  0.23482898f,
         0.1941489f,   0.19488564f,  0.01324697f,  0.02416836f,  0.19106996f,  0.14916961f, }
    },

    { // DenseWeights
        -0.16879508f,-0.76467973f,-0.13576327f, 0.43353727f,
        -0.9420055f,-1.2182641f,  -0.6342814f,  0.16264066f,
    },

    0.17694579f // DenseBias
};

#endif // MODELGAIN1008_H_INCLUDED
