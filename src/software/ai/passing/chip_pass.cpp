#include "software/ai/passing/chip_pass.h"


ChipPass::ChipPass(Point passer_point, Point receiver_point)
    : BasePass(passer_point, receiver_point),
      bounce_ranges()
{   
    pass_length = length();
    first_bounce_range_m = calcFirstBounceRange();
}

double ChipPass::calcFirstBounceRange() 
{
    double last_bounce_range = 0.0;
    double length_to_go = length();

    // if we cover more than 90% of the pass, stop calculating
    while(length_to_go >= pass_length * 0.2)
    {
        double bounce_height = getBounceHeightFromDistanceTraveled(length_to_go);
        last_bounce_range = getBounceRangeFromBounceHeight(bounce_height);
        bounce_ranges.push_back(last_bounce_range);
        length_to_go -= last_bounce_range;
    }

    return last_bounce_range;
}

double ChipPass::getBounceHeightFromDistanceTraveled(double distance_traveled)
{
    return 2 * std::exp(-1.61 * (distance_traveled + 2.82 - pass_length));
}

double ChipPass::getBounceRangeFromBounceHeight(double bounce_height)
{
    return 4 * bounce_height * (std::cos(ROBOT_CHIP_ANGLE_RADIANS) / std::sin(ROBOT_CHIP_ANGLE_RADIANS));
}

double ChipPass::firstBounceRange()
{
    return first_bounce_range_m;
}

Duration ChipPass::estimatePassDuration() const
{
    return Duration::fromSeconds(0);
}

Duration ChipPass::estimateTimeToPoint(Point& point) const
{
    return Duration::fromSeconds(0);
}

std::ostream& operator<<(std::ostream& output_stream, const ChipPass& pass)
{
     output_stream << "Pass from " << pass.passer_point 
                  << " to: " << pass.receiver_point
                  << " w/ First Bounce Range (m/s): " << pass.first_bounce_range_m;

    return output_stream;
}

bool ChipPass::operator==(const ChipPass& other) const
{
    return BasePass::operator==(other) &&
           this->first_bounce_range_m == other.first_bounce_range_m;
}