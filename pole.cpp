#include "pole.h"

Pole::Pole(int nr, QWidget *parent) : QPushButton(parent)
{
    nrPola = nr;
    aktualna_wartosc = BRAK;
    setFixedSize(50,50);
    setStyleSheet("QPushButton { font-size : 26px; }");
}

bool Pole::zaznacz_ruch(Pole::ruch_t nowa_wartosc)
{
    if (aktualna_wartosc != BRAK) {
      return false;
    } else {
        if (nowa_wartosc == KRZYZYK) {
            setText("X");
        } else {
            setText("O");
        }
        aktualna_wartosc = nowa_wartosc;
        return true;
    }
}

void Pole::mouseReleaseEvent(QMouseEvent *e)
{
    emit wybrano(nrPola);
    QAbstractButton::mouseReleaseEvent(e);
}

void Pole::resetuj()
{
    aktualna_wartosc = BRAK;
    setText("");
}
