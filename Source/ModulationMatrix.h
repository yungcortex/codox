#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <array>
#include <unordered_map>

// ModulationMatrix.h - Serum-style modulation routing system
// Allows LFOs, envelopes, and other sources to modulate any parameter

//==============================================================================
// Modulation source types
//==============================================================================
enum class ModSource
{
    LFO1 = 0,
    LFO2 = 1,
    LFO3 = 2,
    LFO4 = 3,
    FilterEnv = 4,
    AmpEnv = 5,
    Velocity = 6,
    ModWheel = 7,
    Note = 8,           // MIDI note number scaled 0-1 (C-2 to G8)
    Aftertouch = 9,
    PitchBend = 10,
    Macro1 = 11,
    Macro2 = 12,
    Macro3 = 13,
    Macro4 = 14,
    Macro5 = 15,
    Macro6 = 16,
    Macro7 = 17,
    Macro8 = 18,
    NumSources
};

// Convert ModSource to display name
inline juce::String getModSourceName(ModSource source)
{
    switch (source)
    {
        case ModSource::LFO1: return "LFO 1";
        case ModSource::LFO2: return "LFO 2";
        case ModSource::LFO3: return "LFO 3";
        case ModSource::LFO4: return "LFO 4";
        case ModSource::FilterEnv: return "Filter Env";
        case ModSource::AmpEnv: return "Amp Env";
        case ModSource::Velocity: return "Velocity";
        case ModSource::ModWheel: return "Mod Wheel";
        case ModSource::Note: return "Note";
        case ModSource::Aftertouch: return "Aftertouch";
        case ModSource::PitchBend: return "Pitch Bend";
        case ModSource::Macro1: return "Macro 1";
        case ModSource::Macro2: return "Macro 2";
        case ModSource::Macro3: return "Macro 3";
        case ModSource::Macro4: return "Macro 4";
        case ModSource::Macro5: return "Macro 5";
        case ModSource::Macro6: return "Macro 6";
        case ModSource::Macro7: return "Macro 7";
        case ModSource::Macro8: return "Macro 8";
        default: return "Unknown";
    }
}

//==============================================================================
// Single modulation routing
//==============================================================================
struct ModRoute
{
    ModSource source;
    juce::String destParamId;
    float depth;        // -1.0 to +1.0 (negative = inverted modulation)
    bool bipolar;       // true = centered modulation, false = unipolar from base value

    // For JSON serialization
    juce::var toVar() const
    {
        auto obj = new juce::DynamicObject();
        obj->setProperty("source", static_cast<int>(source));
        obj->setProperty("dest", destParamId);
        obj->setProperty("depth", depth);
        obj->setProperty("bipolar", bipolar);
        return juce::var(obj);
    }

    static ModRoute fromVar(const juce::var& v)
    {
        ModRoute route;
        if (auto* obj = v.getDynamicObject())
        {
            route.source = static_cast<ModSource>(static_cast<int>(obj->getProperty("source")));
            route.destParamId = obj->getProperty("dest").toString();
            route.depth = static_cast<float>(obj->getProperty("depth"));
            route.bipolar = static_cast<bool>(obj->getProperty("bipolar"));
        }
        return route;
    }
};

//==============================================================================
// Modulation Matrix - manages all modulation routing
//==============================================================================
class ModulationMatrix
{
public:
    static constexpr int MaxRoutes = 64; // Max simultaneous modulation routes

    ModulationMatrix()
    {
        sourceValues.fill(0.0f);
    }

    //==========================================================================
    // Route management
    //==========================================================================

    // Add a new modulation route
    // Returns true if added, false if max routes reached
    bool addRoute(ModSource source, const juce::String& destParamId, float depth, bool bipolar = false)
    {
        if (routes.size() >= MaxRoutes)
            return false;

        // Check if this exact route already exists
        for (auto& route : routes)
        {
            if (route.source == source && route.destParamId == destParamId)
            {
                // Update existing route
                route.depth = depth;
                route.bipolar = bipolar;
                return true;
            }
        }

        routes.push_back({ source, destParamId, depth, bipolar });
        rebuildDestinationCache();
        return true;
    }

    // Remove a specific route
    void removeRoute(ModSource source, const juce::String& destParamId)
    {
        routes.erase(
            std::remove_if(routes.begin(), routes.end(),
                [&](const ModRoute& r) {
                    return r.source == source && r.destParamId == destParamId;
                }),
            routes.end()
        );
        rebuildDestinationCache();
    }

    // Update route depth (for UI dragging)
    void setRouteDepth(ModSource source, const juce::String& destParamId, float newDepth)
    {
        for (auto& route : routes)
        {
            if (route.source == source && route.destParamId == destParamId)
            {
                route.depth = juce::jlimit(-1.0f, 1.0f, newDepth);
                return;
            }
        }
    }

    // Clear all routes to a specific destination
    void clearRoutesForDestination(const juce::String& destParamId)
    {
        routes.erase(
            std::remove_if(routes.begin(), routes.end(),
                [&](const ModRoute& r) {
                    return r.destParamId == destParamId;
                }),
            routes.end()
        );
        rebuildDestinationCache();
    }

    // Clear all routes
    void clearAllRoutes()
    {
        routes.clear();
        destinationCache.clear();
    }

    //==========================================================================
    // Source value updates (call per sample or per block)
    //==========================================================================

    // Set a source value (LFOs: -1 to +1, Envelopes/Velocity/etc: 0 to 1)
    void setSourceValue(ModSource source, float value)
    {
        sourceValues[static_cast<size_t>(source)] = value;
    }

    // Get current source value
    float getSourceValue(ModSource source) const
    {
        return sourceValues[static_cast<size_t>(source)];
    }

    //==========================================================================
    // Modulated value calculation
    //==========================================================================

    // Get the modulated value for a parameter
    // baseValue: the current APVTS parameter value
    // paramMin/paramMax: the parameter's valid range
    // Returns: baseValue + sum of all modulations, clamped to range
    float getModulatedValue(const juce::String& paramId, float baseValue, float paramMin, float paramMax) const
    {
        auto it = destinationCache.find(paramId);
        if (it == destinationCache.end())
            return baseValue; // No modulation for this param

        float totalMod = 0.0f;
        float paramRange = paramMax - paramMin;

        for (int routeIdx : it->second)
        {
            const auto& route = routes[static_cast<size_t>(routeIdx)];
            float sourceVal = sourceValues[static_cast<size_t>(route.source)];

            // Apply depth
            float modAmount = sourceVal * route.depth;

            // For bipolar sources (LFOs output -1 to +1), scale to parameter range
            // For unipolar sources (envelopes output 0 to 1), also scale
            if (route.bipolar)
            {
                // Bipolar: modulation centered on base value
                // depth of 1.0 = modulate ±50% of parameter range
                totalMod += modAmount * (paramRange * 0.5f);
            }
            else
            {
                // Unipolar: modulation from base value upward (or downward if negative depth)
                // depth of 1.0 = modulate up to 100% of parameter range
                totalMod += modAmount * paramRange;
            }
        }

        return juce::jlimit(paramMin, paramMax, baseValue + totalMod);
    }

    // Get total modulation offset for a parameter (for UI visualization)
    // Returns normalized value (-1 to +1) relative to parameter range
    float getModulationAmount(const juce::String& paramId, float paramRange) const
    {
        auto it = destinationCache.find(paramId);
        if (it == destinationCache.end())
            return 0.0f;

        float totalMod = 0.0f;

        for (int routeIdx : it->second)
        {
            const auto& route = routes[static_cast<size_t>(routeIdx)];
            float sourceVal = sourceValues[static_cast<size_t>(route.source)];
            float modAmount = sourceVal * route.depth;

            if (route.bipolar)
                totalMod += modAmount * 0.5f;
            else
                totalMod += modAmount;
        }

        return totalMod; // Normalized modulation amount
    }

    // Check if a parameter has any modulation assigned
    bool hasModulation(const juce::String& paramId) const
    {
        return destinationCache.find(paramId) != destinationCache.end();
    }

    // Get routes for a specific destination (for UI)
    std::vector<ModRoute> getRoutesForDestination(const juce::String& paramId) const
    {
        std::vector<ModRoute> result;
        for (const auto& route : routes)
        {
            if (route.destParamId == paramId)
                result.push_back(route);
        }
        return result;
    }

    //==========================================================================
    // Accessors
    //==========================================================================

    const std::vector<ModRoute>& getRoutes() const { return routes; }
    size_t getRouteCount() const { return routes.size(); }

    //==========================================================================
    // Serialization (for preset save/load)
    //==========================================================================

    juce::String toJSON() const
    {
        juce::Array<juce::var> arr;
        for (const auto& route : routes)
            arr.add(route.toVar());
        return juce::JSON::toString(arr);
    }

    void fromJSON(const juce::String& json)
    {
        routes.clear();
        auto parsed = juce::JSON::parse(json);
        if (auto* arr = parsed.getArray())
        {
            for (const auto& item : *arr)
            {
                routes.push_back(ModRoute::fromVar(item));
            }
        }
        rebuildDestinationCache();
    }

    juce::ValueTree getState() const
    {
        juce::ValueTree state("ModMatrix");
        state.setProperty("json", toJSON(), nullptr);
        return state;
    }

    void setState(const juce::ValueTree& state)
    {
        if (state.hasProperty("json"))
            fromJSON(state.getProperty("json").toString());
    }

private:
    std::vector<ModRoute> routes;
    std::array<float, static_cast<size_t>(ModSource::NumSources)> sourceValues;

    // Cache: destination param ID -> list of route indices for fast lookup
    std::unordered_map<juce::String, std::vector<int>> destinationCache;

    void rebuildDestinationCache()
    {
        destinationCache.clear();
        for (size_t i = 0; i < routes.size(); ++i)
        {
            destinationCache[routes[i].destParamId].push_back(static_cast<int>(i));
        }
    }
};
