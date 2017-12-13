#ifndef POLE_H
#define POLE_H
#include <QPushButton>
#include <QDebug>

class Pole : public QPushButton
{
    Q_OBJECT
public:
    typedef enum
    {
      BRAK    = 0,
      KOLKO   = 1,
      KRZYZYK = 2
    } ruch_t;

    typedef enum
    {
      G_KOLKO   = 0,
      G_KRZYZYK = 1
    } gracz_t;

    typedef enum
    {
      WYGRYWA_KOLKO     = -1,
      REMIS             =  0,
      WYGRYWA_KRZYZYK   =  1,
      NIEROZSTRZYGNIETA =  2
    } rezultat_t;

    Pole(int nr, QWidget *parent = nullptr);
    bool zaznacz_ruch(ruch_t nowa_wartosc);
    void mouseReleaseEvent(QMouseEvent *e);

    ruch_t aktualna_wartosc;
    void resetuj();
    int nrPola;

private:


signals:
    void wybrano(int nr);

public slots:

};

#endif // POLE_H
