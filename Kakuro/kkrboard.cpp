#include "kkrboard.h"
#include <QPainter>
#include <QPalette>
#include <QFont>
#include <QDebug>
#include <array>
#include "inputfactory.h"

KkrBoard::KkrBoard(QWidget *parent)
    : QWidget(parent)
    , m_fontAns(FONT_ANS)
    , m_fontClue(FONT_CLUE)
    , m_showDigits(true)
{
    m_pCellInput = cellInputFactory(this);
    m_fontAns.setPixelSize(CELL_WIDTH);
    m_fontClue.setPixelSize(CLUE_WIDTH);
}

KkrBoard::~KkrBoard()
{
    cellInputCleanup();
}

/*
 * public slots
 */
void KkrBoard::updateProblem(std::shared_ptr<problemdata::ProblemData> pNewData)
{
    m_showDigits = true;
    m_pData = pNewData;

    // calculate sizes
    m_inner_width = (CELL_WIDTH + BORDER_THICK) * m_pData->getNumCols() - BORDER_THICK;
    m_inner_height = (CELL_WIDTH + BORDER_THICK) * m_pData->getNumRows() - BORDER_THICK;
    m_frame_width = 2*FRAME_THICK + m_inner_width;
    m_frame_height = 2*FRAME_THICK + m_inner_height;
    m_board_width = 2*MARGIN + m_frame_width;
    m_board_height = 2*MARGIN + m_frame_height;

    setFixedSize(m_board_width, m_board_height);

    update();
}

void KkrBoard::updateStatus(playstatus::Status newStatus)
{
    switch(newStatus) {
    case playstatus::Status::INPLAY:
    case playstatus::Status::DONE:
        m_showDigits = true;
        break;
    default:
        m_showDigits = false;
    }
    update();
}

void KkrBoard::updateUserAnswer(useranswer::SharedAnswer pNewAns)
{
    m_pAns = pNewAns;
    // no update();
}

void KkrBoard::updateAnswer(QPoint cellPos)
{
    qDebug() << "KkrBoard::updateCell:" << cellPos;
    update();
}

void KkrBoard::cellInput(int value)
{
    ua::CellData cd;
    cd.p.setX(m_inCol); cd.p.setY(m_inRow);
    cd.answer = value;

    qDebug() << "KkrBoard::cellInputFromUser:" << cd.p << ' ' << value;
    emit updateCellAnswer(cd);
}

/*
 * regular methods
 */
QRect KkrBoard::getCellRect(int col, int row) const
{
    const int x = MARGIN + FRAME_THICK + col * (CELL_WIDTH + BORDER_THICK);
    const int y = MARGIN + FRAME_THICK + row * (CELL_WIDTH + BORDER_THICK);
    return QRect(x, y, CELL_WIDTH, CELL_WIDTH);
}

QRect KkrBoard::getClueRectRight(const QRect &cellRect) const
{
    const QPoint topRight{cellRect.topRight()};
    const int x = topRight.x() - CLUE_WIDTH - 1;
    const int y = topRight.y() + 2;
    return QRect(x, y, CLUE_WIDTH, CLUE_WIDTH);
}

QRect KkrBoard::getClueRectDown(const QRect &cellRect) const
{
    const QPoint bottomLeft{cellRect.bottomLeft()};
    const int x = bottomLeft.x() + 2;
    const int y = bottomLeft.y() - CLUE_WIDTH - 1;
    return QRect(x, y, CLUE_WIDTH, CLUE_WIDTH);
}

QPoint KkrBoard::getCellCoord(int x, int y) const
{
    QPoint ret(-1, -1);
    static const int ORIGIN = MARGIN + FRAME_THICK;
    static const int CELL_FRAME_WIDTH = CELL_WIDTH + BORDER_THICK;

    x -= ORIGIN; y -= ORIGIN;
    if(x < 0 || y < 0)
        return ret;

    if(x % CELL_FRAME_WIDTH == CELL_WIDTH || y % CELL_FRAME_WIDTH == CELL_WIDTH)
        return ret; // on the cell border

    const int col = x / CELL_FRAME_WIDTH;
    const int row = y / CELL_FRAME_WIDTH;
    if(col >= m_pData->getNumCols() || row >= m_pData->getNumRows())
        return ret;

    ret.setX(col); ret.setY(row);

    return ret;
}

void KkrBoard::drawCell(QPainter &p, int col, int row) const
{
    static const char * digits[] = {
        "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
        "40", "41", "42", "43", "44", "45"
    };
    const QRect cellRect{getCellRect(col, row)};
    switch(m_pData->getCellType(col, row)) {
    case pd::CellType::CellAnswer:
        if(m_showDigits) {
            const int ans = m_pAns->getAnswer(col, row);
            if(ans != ua::ANSWER_NODATA) {
                p.setFont(m_fontAns);
                p.drawText(cellRect, Qt::AlignCenter | Qt::AlignHCenter, digits[ans]);
            }
        }
        break;
    case pd::CellType::CellClue:
    {
        p.setBrush(Qt::SolidPattern);
        std::array<QPoint, 3> points;

        // upper right triangle
        points[0] = cellRect.topLeft(); points[0] += QPoint(2,1);
        points[1] = cellRect.topRight(); points[1] += QPoint(-1,1);
        points[2] = cellRect.bottomRight(); points[2] += QPoint(-1,-2);
        p.drawPolygon(points.data(), static_cast<int>(points.size()));

        // bottom left triangle
        points[0] = cellRect.topLeft(); points[0] += QPoint(1,2);
        points[1] = cellRect.bottomLeft(); points[1] += QPoint(1,-1);
        points[2] = cellRect.bottomRight(); points[2] += QPoint(-2,-1);
        p.drawPolygon(points.data(), static_cast<int>(points.size()));

        QBrush brWhite(Qt::white);
        if(m_pData->getClueRight(col, row) != pd::CLOSED_CLUE) {
            const QRect clueRect{getClueRectRight(cellRect)};
            p.fillRect(clueRect, brWhite);
            if(m_showDigits) {
                 p.setFont(m_fontClue);
                 p.drawText(clueRect, Qt::AlignCenter | Qt::AlignHCenter,
                            digits[m_pData->getClueRight(col, row)]);
            }
        }

        if(m_pData->getClueDown(col, row) != pd::CLOSED_CLUE) {
            const QRect clueRect{getClueRectDown(cellRect)};
            p.fillRect(clueRect, brWhite);
            if(m_showDigits) {
                 p.setFont(m_fontClue);
                 p.drawText(clueRect, Qt::AlignCenter | Qt::AlignHCenter,
                            digits[m_pData->getClueDown(col, row)]);
            }
        }
    }
        break;
    default:
        Q_ASSERT(false);
    }
}

void KkrBoard::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // frame
    p.fillRect(MARGIN, MARGIN, m_frame_width, FRAME_THICK, Qt::SolidPattern);
    p.fillRect(MARGIN, MARGIN+m_frame_height-FRAME_THICK, m_frame_width, FRAME_THICK, Qt::SolidPattern);
    p.fillRect(MARGIN, MARGIN, FRAME_THICK, m_frame_height, Qt::SolidPattern);
    p.fillRect(MARGIN+m_frame_width-FRAME_THICK, MARGIN, FRAME_THICK, m_frame_height, Qt::SolidPattern);

    // cell borders
    const int TL = MARGIN + FRAME_THICK;
    for(int yi = 1; yi < m_pData->getNumRows(); ++yi) {
        const int y = TL - BORDER_THICK + yi * (BORDER_THICK+CELL_WIDTH);
        p.fillRect(TL, y, m_inner_width, BORDER_THICK, Qt::SolidPattern);
    }

    for(int xi = 1; xi < m_pData->getNumCols(); ++xi) {
        const int x = TL - BORDER_THICK + xi * (BORDER_THICK+CELL_WIDTH);
        p.fillRect(x, TL, BORDER_THICK, m_inner_height, Qt::SolidPattern);
    }

    // cell contents
    for(int y = 0; y < m_pData->getNumRows(); ++y) {
        for(int x = 0; x < m_pData->getNumCols(); ++x) {
            drawCell(p, x, y);
        }
    }
}

void KkrBoard::mousePressEvent(QMouseEvent *e)
{
    if(m_showDigits && e->button() == Qt::RightButton) {
        const QPoint pt = getCellCoord(e->x(), e->y());
        if(pt.x() >= 0 && m_pData->getCellType(pt.x(), pt.y()) == pd::CellType::CellAnswer) {
            m_inCol = pt.x(); m_inRow = pt.y();
            m_inValue = m_pAns->getAnswer(pt.x(), pt.y());
            qDebug() << "KkrBoard activateing cell input";
            const QRect cellRect = getCellRect(pt);
            m_pCellInput->move(cellRect.x(), cellRect.y());
            m_pCellInput->setEnabled(true);
            m_pCellInput->setVisible(true);
        }
    }
}
