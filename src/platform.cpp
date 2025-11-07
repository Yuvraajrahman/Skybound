#include "platform.h"

#include <cmath>

void UpdatePlatforms(std::vector<Platform>& platforms, float dt)
{
    for (Platform& platform : platforms)
    {
        if (!platform.moving || platform.travelTime <= 0.0f)
        {
            continue;
        }

        platform.timer += dt;
        const float duration = platform.travelTime;
        const float cycle = std::fmod(platform.timer, duration * 2.0f);
        float t = cycle / duration;
        if (t > 1.0f)
        {
            t = 2.0f - t;
        }

        const Vector2 delta{platform.endPosition.x - platform.startPosition.x,
                            platform.endPosition.y - platform.startPosition.y};

        platform.bounds.x = platform.startPosition.x + delta.x * t;
        platform.bounds.y = platform.startPosition.y + delta.y * t;
    }
}


