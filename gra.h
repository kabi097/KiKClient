#ifndef GRA_H
#define GRA_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTextEdit>
#include <QScrollBar>
#include "pole.h"

class Gra : public QMainWindow
{
    Q_OBJECT

public:
    typedef enum
    {
        POTWIERDZENIE,
        WIAD_TEKST,
        RUCH,
        PODDAJ,
        PLANSZA
    } type_t;

    typedef struct {
        Pole::ruch_t ruchy[9];
    } plansza_t;

    struct wiadomosc
    {
        uint8_t sync;
        uint8_t type;
        uint8_t length;
        union
        {
            struct
            {
                uint8_t poddalSie;
            } poddanie;

            struct
            {
                uint8_t rezultat;
            } potwierdzenie;

            struct
            {
                char napis[80];
            } czat;

            struct
            {
                uint8_t x;
                uint8_t y;
            } ruch;

            struct {
                plansza_t plansza;
            } mojaMapa;
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

    //---------------
    typedef enum
    {
      KLIENT,
      SERWER,
      INFO
    } czat_t;
    //--------------

    explicit Gra(QWidget *parent = 0);

    rezultat_t rezultat_gry();
    //int ocen_rezultat();
    void zmien_gracza();
    void czysc_plansze();
    QByteArray IntToArray(qint32 source);
    void poddajSie();
    void dodajCzat(czat_t rodzaj, QString tekst);
    void koniecGry(rezultat_t wynik);

    int nrPola;

    ~Gra();

private:
    QTcpSocket *socket;
    Pole **pole;
    rezultat_t rezultat;
    gracz_t aktualny_gracz;
    QPushButton *connectButton;
    QLineEdit *address;
    QLineEdit *port;
    QLineEdit *czatin;
    QTextEdit *czatout;

public slots:
    void przyciskClicked();
    void polacz();
    void rozlacz();
    void czytajDane();
    void wyslijTekst();
    void wybranoPole(int i);
};

#endif // GRA_H
