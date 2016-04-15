#include "playstatus.h"

namespace playstatus {

PlayStatus::PlayStatus()
    : m_status(Status::NODATA)
    , m_timeOffset(0)
{
}

qint64 PlayStatus::getElapsedTime() const {
    if(m_status == Status::INPLAY)
        return m_time.elapsed() + m_timeOffset;

    return m_timeOffset;
}

/*
 * slots
 */
void PlayStatus::updateProblem(std::shared_ptr<problemdata::ProblemData> /*pNewData*/)
{
    m_status = Status::READY;
    m_timeOffset = 0;
    emit statusChanged(m_status);
}

void PlayStatus::playPressed()
{
    switch(m_status) {
    case Status::READY:
    case Status::PAUSED:
        m_time.start();
        m_status = Status::INPLAY;
        break;
    case Status::INPLAY:
        m_timeOffset += m_time.elapsed();
        m_time.invalidate();
        m_status = Status::PAUSED;
        break;
    default:
        return;
    }
    emit statusChanged(m_status);
}

void PlayStatus::solved()
{
    if(m_status == Status::INPLAY) {
        m_timeOffset += m_time.elapsed();
        m_time.invalidate();
    }
    m_status = Status::DONE;
    emit statusChanged(m_status);
}

void PlayStatus::giveup()
{
    if(m_status != Status::INPLAY && m_status != Status::PAUSED)
        return;

    if(m_status == Status::INPLAY) {
        m_timeOffset += m_time.elapsed();
        m_time.invalidate();
    }
    m_status = Status::DONE;
    emit statusChanged(m_status);
}

}   // namespace playstatus
