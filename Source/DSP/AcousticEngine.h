#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "AnalogModules.h"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <atomic>

//==============================================================================
// Filtro All-Pass de dispersión (evita choques de fase en micrófonos de ambiente)
//==============================================================================
class SimpleAllPass
{
public:
    void prepare (size_t delaySamples)
    {
        size_t sz = 1;
        while (sz < delaySamples + 8) sz <<= 1;
        buf.assign (sz, 0.0f);
        mask = sz - 1;
        delay = delaySamples;
        pos = 0;
    }

    void reset() { std::fill (buf.begin(), buf.end(), 0.0f); pos = 0; }

    inline float process (float x, float g = 0.52f) noexcept
    {
        const float d = buf[(pos - delay) & mask];
        const float v = x - g * d;
        buf[pos] = v;
        pos = (pos + 1) & mask;
        return d + g * v;
    }

private:
    std::vector<float> buf;
    size_t mask = 0, delay = 1, pos = 0;
};

//==============================================================================
// Red de Retardos Realimentados Hadamard de 8 Canales (Simétrica en L y R)
//==============================================================================
class HadamardFDN
{
public:
    static constexpr int NumLines = 8;

    void prepare (double sampleRate)
    {
        sr = sampleRate > 0.0 ? sampleRate : 44100.0;
        size_t needed = static_cast<size_t> (sr * 0.25);
        bufSize = 1;
        while (bufSize < needed) bufSize <<= 1;
        mask = bufSize - 1;

        for (auto& line : lines) line.assign (bufSize, 0.0f);
        reset();
    }

    void reset()
    {
        for (auto& line : lines) std::fill (line.begin(), line.end(), 0.0f);
        lpState.fill (0.0f); dcIn.fill (0.0f); dcOut.fill (0.0f);
        writePos = 0;
        for (int i = 0; i < NumLines; ++i)
            lfoPhase[static_cast<size_t> (i)] = static_cast<float> (i) * 0.785f;
    }

    void setProfile (const std::array<float, NumLines>& delayMs, float rt60Seconds,
                     float dampingAmount, float modDepthMs)
    {
        rt60 = std::clamp (rt60Seconds, 0.18f, 7.5f);
        damp = std::clamp (dampingAmount, 0.05f, 0.88f);
        modDepthSamples = modDepthMs * 0.001f * static_cast<float> (sr);

        for (size_t i = 0; i < NumLines; ++i)
        {
            baseDelaySamples[i] = std::clamp (delayMs[i] * 0.001f * static_cast<float> (sr),
                                              32.0f, static_cast<float> (bufSize - 64));
            const float delaySec = baseDelaySamples[i] / static_cast<float> (sr);
            lineGain[i] = std::min (0.988f, std::pow (10.0f, -3.0f * delaySec / rt60));
        }
    }

    inline void processStereo (float inL, float inR, float& outL, float& outR) noexcept
    {
        static constexpr std::array<float, NumLines> lfoRates {
            0.17f, 0.23f, 0.29f, 0.37f, 0.41f, 0.47f, 0.53f, 0.61f
        };

        std::array<float, NumLines> delayed {};

        for (size_t i = 0; i < NumLines; ++i)
        {
            lfoPhase[i] += juce::MathConstants<float>::twoPi * lfoRates[i] / static_cast<float> (sr);
            if (lfoPhase[i] > juce::MathConstants<float>::twoPi)
                lfoPhase[i] -= juce::MathConstants<float>::twoPi;

            const float d = baseDelaySamples[i] + std::sin (lfoPhase[i]) * modDepthSamples;
            const auto dInt = static_cast<size_t> (d);
            const float frac = d - static_cast<float> (dInt);

            const size_t idx0 = (writePos - dInt) & mask;
            const size_t idx1 = (idx0 - 1) & mask;
            const float sample = lines[i][idx0] * (1.0f - frac) + lines[i][idx1] * frac;

            lpState[i] = sample * (1.0f - damp) + lpState[i] * damp;
            const float dc = lpState[i] - dcIn[i] + 0.996f * dcOut[i];
            dcIn[i] = lpState[i];
            dcOut[i] = dc;
            delayed[i] = dc * lineGain[i];
        }

        const float a0 = delayed[0] + delayed[1], a1 = delayed[0] - delayed[1];
        const float a2 = delayed[2] + delayed[3], a3 = delayed[2] - delayed[3];
        const float a4 = delayed[4] + delayed[5], a5 = delayed[4] - delayed[5];
        const float a6 = delayed[6] + delayed[7], a7 = delayed[6] - delayed[7];

        const float b0 = a0 + a2, b1 = a1 + a3, b2 = a0 - a2, b3 = a1 - a3;
        const float b4 = a4 + a6, b5 = a5 + a7, b6 = a4 - a6, b7 = a5 - a7;

        constexpr float norm = 0.35355339059f;
        std::array<float, NumLines> mixed {
            (b0 + b4) * norm, (b1 + b5) * norm, (b2 + b6) * norm, (b3 + b7) * norm,
            (b0 - b4) * norm, (b1 - b5) * norm, (b2 - b6) * norm, (b3 - b7) * norm
        };

        // Inyección y extracción simétricas en energía y retardo promedio entre L y R
        const float sL = inL * 0.35f, sR = inR * 0.35f;
        lines[0][writePos] = mixed[0] + sL; lines[1][writePos] = mixed[1] + sR;
        lines[2][writePos] = mixed[2] + sR; lines[3][writePos] = mixed[3] + sL;
        lines[4][writePos] = mixed[4] - sL; lines[5][writePos] = mixed[5] - sR;
        lines[6][writePos] = mixed[6] - sR; lines[7][writePos] = mixed[7] - sL;
        writePos = (writePos + 1) & mask;

        outL = 0.32f * (delayed[0] - delayed[3] + delayed[5] - delayed[6]);
        outR = 0.32f * (delayed[1] - delayed[2] + delayed[4] - delayed[7]);
    }

private:
    double sr = 44100.0;
    size_t bufSize = 16384, mask = 16383, writePos = 0;
    float rt60 = 1.5f, damp = 0.35f, modDepthSamples = 8.0f;
    std::array<std::vector<float>, NumLines> lines;
    std::array<float, NumLines> baseDelaySamples {}, lineGain {}, lpState {}, dcIn {}, dcOut {}, lfoPhase {};
};

//==============================================================================
// Configuración de los 4 Atriles de Micrófono en la Sala
//==============================================================================
struct MultiMicRoomConfig
{
    int modeIdx = 1;       // 0: Reverb, 1: Re-Mic
    int sourceIdx = 0;     // 0: Vocals, 1: Drums, 2: Ensemble, 3: Monitors
    int roomIdx = 0;       // 0: Studio A (Shangri-La), 1: Studio B (Canyon)
    int wallsLive = 1;     // 0: Tight, 1: Live
    bool phaseSafe = true;
    bool phaseInvert = false;

    std::array<float, 4> micX    { 0.0f, -0.38f, 0.42f, 0.0f };
    std::array<float, 4> micY    { 0.18f, 0.52f, 0.82f, 0.62f };
    std::array<float, 4> micGain { 1.0f,  0.45f, 0.35f, 0.30f };
};

//==============================================================================
// Motor Acústico Multi-Atril + Protección de Fase + Cámaras + Custom WAV IR
//==============================================================================
class AcousticEngine
{
public:
    void prepare (double sampleRate, int maxBlockSize)
    {
        sr = sampleRate > 0.0 ? sampleRate : 44100.0;

        size_t needed = static_cast<size_t> (sr * 0.25);
        erBufSize = 1;
        while (erBufSize < needed) erBufSize <<= 1;
        erMask = erBufSize - 1;

        erBufL.assign (erBufSize, 0.0f);
        erBufR.assign (erBufSize, 0.0f);
        erWritePos = 0;

        roomFdn.prepare (sr);
        chamberFdn.prepare (sr);

        midPhaseHpf.prepare (sr);   midPhaseHpf.setHighPass (150.0f, 0.7071f);
        airPhaseHpf.prepare (sr);   airPhaseHpf.setHighPass (240.0f, 0.7071f);
        deccaPhaseHpf.prepare (sr); deccaPhaseHpf.setHighPass (120.0f, 0.7071f);

        apMidL.prepare (static_cast<size_t> (sr * 0.0053));
        apMidR.prepare (static_cast<size_t> (sr * 0.0067));
        apAirL.prepare (static_cast<size_t> (sr * 0.0113));
        apAirR.prepare (static_cast<size_t> (sr * 0.0137));

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sr;
        spec.maximumBlockSize = static_cast<juce::uint32> (maxBlockSize);
        spec.numChannels = 2;

        preDelay.prepare (spec);
        customConvolver.prepare (spec);
        airLpL = airLpR = 0.0f;
    }

    void reset()
    {
        std::fill (erBufL.begin(), erBufL.end(), 0.0f);
        std::fill (erBufR.begin(), erBufR.end(), 0.0f);
        erWritePos = 0;
        roomFdn.reset(); chamberFdn.reset();
        midPhaseHpf.reset(); airPhaseHpf.reset(); deccaPhaseHpf.reset();
        apMidL.reset(); apMidR.reset(); apAirL.reset(); apAirR.reset();
        preDelay.reset(); customConvolver.reset();
    }

    void loadCustomImpulseResponse (const juce::File& file)
    {
        if (! file.existsAsFile()) return;
        customConvolver.loadImpulseResponse (file,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::yes,
                                             0);
        customIRLoaded.store (true);
        loadedIRName = file.getFileNameWithoutExtension();
    }

    juce::String getLoadedIRName() const { return loadedIRName; }

    void processMultiMicRoom (juce::AudioBuffer<float>& buffer, const MultiMicRoomConfig& cfg)
    {
        if (buffer.getNumChannels() < 2) return;

        static constexpr std::array<float, 8> studioADelays { 29.3f, 37.1f, 43.7f, 53.3f, 61.9f, 71.1f, 83.3f, 97.7f };
        static constexpr std::array<float, 8> studioBDelays { 17.3f, 21.1f, 26.9f, 31.7f, 37.9f, 43.1f, 51.3f, 59.7f };

        const float baseRt60 = (cfg.roomIdx == 0) ? (cfg.wallsLive ? 1.85f : 1.15f)
                                                  : (cfg.wallsLive ? 1.05f : 0.62f);
        const float damp = cfg.wallsLive ? 0.26f : 0.56f;
        roomFdn.setProfile (cfg.roomIdx == 0 ? studioADelays : studioBDelays, baseRt60, damp, 0.35f);

        const float roomW = (cfg.roomIdx == 0) ? 16.0f : 10.5f;
        const float roomD = (cfg.roomIdx == 0) ? 12.0f : 8.5f;
        const float speed = 343.0f;
        const float wallRef = cfg.wallsLive ? 0.68f : 0.42f;

        std::array<float, 4> distMeters {};
        std::array<float, 4> panL {}, panR {};
        std::array<float, 4> delaySamples {};

        for (size_t s = 0; s < 4; ++s)
        {
            const float mx = cfg.micX[s] * (roomW * 0.38f);
            const float my = juce::jmap (cfg.micY[s], 0.0f, 1.0f, 1.2f, roomD - 1.2f);
            distMeters[s] = std::hypot (mx, my);

            const float pan = std::clamp (cfg.micX[s] * 0.65f, -0.85f, 0.85f);
            const float ang = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
            panL[s] = std::cos (ang) * 1.414f;
            panR[s] = std::sin (ang) * 1.414f;

            if (cfg.phaseSafe)
            {
                if (s == 0)
                    delaySamples[s] = 0.0f;
                else
                {
                    const float minHaasSec = (s == 2) ? 0.024f : 0.016f;
                    const float physicalSec = distMeters[s] / speed;
                    delaySamples[s] = std::max (minHaasSec, physicalSec) * static_cast<float> (sr);
                }
            }
            else
            {
                const float relDist = std::max (0.0f, distMeters[s] - distMeters[0]);
                delaySamples[s] = (relDist / speed) * static_cast<float> (sr);
            }
        }

        // Reflexiones tempranas 100% simétricas en espejo (cero sesgo hacia un lado)
        const size_t erNear = static_cast<size_t> (0.0145f * sr);
        const size_t erFar  = static_cast<size_t> (0.0195f * sr);
        const size_t erCeil = static_cast<size_t> (0.0245f * sr);
        const size_t erBack = static_cast<size_t> (0.0320f * sr);

        const float deccaSideOffset = 0.0028f * static_cast<float> (sr);
        const float polaritySign = cfg.phaseInvert ? -1.0f : 1.0f;

        auto* left  = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);
        const int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            const float dryL = left[i];
            const float dryR = right[i];

            float srcL = dryL, srcR = dryR;
            if (cfg.sourceIdx == 0) // Vocals: fuente puntual centrada
            {
                srcL = srcR = 0.5f * (dryL + dryR);
            }
            else if (cfg.sourceIdx == 3) // Monitors L/R: cruce acústico natural en el aire del cuarto
            {
                srcL = dryL * 0.82f + dryR * 0.18f;
                srcR = dryR * 0.82f + dryL * 0.18f;
            }

            erBufL[erWritePos] = srcL;
            erBufR[erWritePos] = srcR;

            const float erL = (erBufL[(erWritePos - erNear) & erMask] * 0.42f
                             + erBufR[(erWritePos - erFar)  & erMask] * 0.32f
                             + erBufL[(erWritePos - erCeil) & erMask] * 0.30f
                             + erBufR[(erWritePos - erBack) & erMask] * 0.24f) * wallRef;

            const float erR = (erBufR[(erWritePos - erNear) & erMask] * 0.42f
                             + erBufL[(erWritePos - erFar)  & erMask] * 0.32f
                             + erBufR[(erWritePos - erCeil) & erMask] * 0.30f
                             + erBufL[(erWritePos - erBack) & erMask] * 0.24f) * wallRef;

            float fdnL = 0.0f, fdnR = 0.0f;
            roomFdn.processStereo (srcL + erL * 0.35f, srcR + erR * 0.35f, fdnL, fdnR);

            float mixL = 0.0f, mixR = 0.0f;

            // ATRIL 1: MAIN (centrado en Vocals y en Monitors cuando mic1X == 0)
            if (cfg.micGain[0] > 0.001f)
            {
                const float g = cfg.micGain[0];
                const float yDist = cfg.micY[0];
                const float dirW = juce::jmap (yDist, 0.0f, 1.0f, 0.94f, 0.35f);
                const float ambW = juce::jmap (yDist, 0.0f, 1.0f, 0.14f, 0.65f);

                const float dirL = cfg.phaseSafe ? srcL : readFrac (erBufL, delaySamples[0]);
                const float dirR = cfg.phaseSafe ? srcR : readFrac (erBufR, delaySamples[0]);

                const float sL = dirL * dirW + (erL * 0.7f + fdnL * 0.5f) * ambW;
                const float sR = dirR * dirW + (erR * 0.7f + fdnR * 0.5f) * ambW;
                mixL += sL * panL[0] * g;
                mixR += sR * panR[0] * g;
            }

            // ATRIL 2: MID (simétrico en retardo, paneo definido por su posición X)
            if (cfg.micGain[1] > 0.001f)
            {
                const float g = cfg.micGain[1] * 0.78f;
                const float yDist = cfg.micY[1];
                float mL = readFrac (erBufL, delaySamples[1]) * (1.0f - yDist * 0.55f) * 0.45f + erL * 0.85f + fdnL * 0.65f;
                float mR = readFrac (erBufR, delaySamples[1]) * (1.0f - yDist * 0.55f) * 0.45f + erR * 0.85f + fdnR * 0.65f;

                if (cfg.phaseSafe)
                {
                    mL = apMidL.process (midPhaseHpf.processSample (0, mL));
                    mR = apMidR.process (midPhaseHpf.processSample (1, mR));
                }
                mixL += mL * panL[1] * g;
                mixR += mR * panR[1] * g;
            }

            // ATRIL 3: FAR / AIR (L->L y R->R corregidos)
            if (cfg.micGain[2] > 0.001f)
            {
                const float g = cfg.micGain[2] * 0.82f;
                float aL = readFrac (erBufL, delaySamples[2]) * 0.25f + erL * 0.55f + fdnL * 1.15f;
                float aR = readFrac (erBufR, delaySamples[2]) * 0.25f + erR * 0.55f + fdnR * 1.15f;

                const float airCoeff = juce::jmap (cfg.micY[2], 0
