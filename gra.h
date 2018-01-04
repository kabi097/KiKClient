#ifndef GRA_H
#define GRA_H

#include <QMainWindow>
#include <QGridLayout>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QTcpSocket>
#include "pole.h"

class Gra : public QMainWindow
{
    Q_OBJECT

public:
    typedef enum
    {
      POTWIERDZENIE,
      WIAD_TEKST,
      RUCH
    } type_t;

    struct wiadomosc
    {
      qint8 type;
      qint8 len;
      union
      {
        struct
        {
          qint8 rezultat;
        } potw;

        struct
        {
          char napis[80];
        } wiadomosc;

        struct
        {
          qint8 number;
        } ruch;
      } dane;
    }  __attribute__ ((packed));

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

    explicit Gra(QWidget *parent = 0);

    rezultat_t rezultat_gry();
    //int ocen_rezultat();
    void zmien_gracza();
    void czysc_plansze();
    QByteArray IntToArray(qint32 source);

    int nrPola;

    ~Gra();

private:
    QTcpSocket *socket;
    Pole **pole;
    rezultat_t rezultat;
    gracz_t aktualny_gracz;

public slots:
    void wybranoPole(int i);
};

#endif // GRA_H
