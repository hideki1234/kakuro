#ifndef USERANSWERMANAGER_H
#define USERANSWERMANAGER_H

#include <QObject>
#include <QPoint>
#include <memory>
#include "useranswer.h"
#include "problemdata.h"

namespace pd = problemdata;

namespace useranswer {

using SharedAnswer = std::shared_ptr<UserAnswer>;

struct CellData {
    QPoint p;
    int answer;
};

class UserAnswerManager : public QObject
{
    Q_OBJECT
    std::shared_ptr<problemdata::ProblemData> m_pProblem;
    std::shared_ptr<UserAnswer> m_pAnswer;

public:
    UserAnswerManager();
    virtual ~UserAnswerManager();

    UserAnswerManager(const UserAnswerManager&) = delete;
    UserAnswerManager &operator=(const UserAnswerManager&) = delete;

    bool isSolved() const;

signals:
    void newUserAnswer(SharedAnswer pNewAns);
    void newCellAnswer(QPoint cellPos);

public slots:
    void updateProblem(std::shared_ptr<pd::ProblemData> pNewData);
    void updateCellAnswer(CellData cellData);
    void deleteCellAnswer(QPoint p);
};

}   // namespace useranswer

#endif // USERANSWERMANAGER_H
