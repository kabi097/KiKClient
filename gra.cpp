#include "gra.h"

Gra::Gra(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Kółko i krzyżyk");
    QWidget *plansza = new QWidget(this);
    QGridLayout *siatka = new QGridLayout;

    int col=0;
    pole = new Pole*[9];
    for (int i=0; i<9; i++) {
        pole[i] = new Pole(i);
        connect(pole[i],SIGNAL(wybrano(int)),this,SLOT(wybranoPole(int)));
        siatka->addWidget(pole[i],i%3,col);
        if (i%3==2) col++;
    }

    rezultat = NIEROZSTRZYGNIETA;
    aktualny_gracz = G_KOLKO;

    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1",2222);
    if (socket->waitForConnected()) {
        qDebug() << "Połączono z serwerem";
    } else {
        qDebug() << "Nie udało się połączyć z serwerem";
    }

    plansza->setLayout(siatka);
    setCentralWidget(plansza);
}

Gra::rezultat_t Gra::rezultat_gry()
{
    //Sprawdzanie wierszy
    for (int y=0; y<3; y++)
    {
      if ((pole[3*y]->aktualna_wartosc == pole[3*y + 1]->aktualna_wartosc) && (pole[3*y]->aktualna_wartosc == pole[3*y + 2]->aktualna_wartosc))
      {
        if (pole[3*y]->aktualna_wartosc  == Pole::KOLKO)
          return WYGRYWA_KOLKO;
        if (pole[3*y]->aktualna_wartosc  == Pole::KRZYZYK)
          return WYGRYWA_KRZYZYK;
      }
    }
    //Sprawdzanie kolumn
    for (int x=0; x<3; x++)
    {
      if ((pole[x]->aktualna_wartosc == pole[x + 3]->aktualna_wartosc) && (pole[x]->aktualna_wartosc == pole[x + 6]->aktualna_wartosc))
      {
        if (pole[x]->aktualna_wartosc  == Pole::KOLKO)
          return WYGRYWA_KOLKO;
        if (pole[x]->aktualna_wartosc  == Pole::KRZYZYK)
          return WYGRYWA_KRZYZYK;
      }
    }
    //Sprawdzanie przekątnych
    if ((pole[0]->aktualna_wartosc == pole[4]->aktualna_wartosc) && (pole[0]->aktualna_wartosc == pole[8]->aktualna_wartosc))
    {
      if (pole[4]->aktualna_wartosc  == Pole::KOLKO)
        return WYGRYWA_KOLKO;
      if (pole[4]->aktualna_wartosc  == Pole::KRZYZYK)
        return WYGRYWA_KRZYZYK;
    }
    if ((pole[2]->aktualna_wartosc == pole[4]->aktualna_wartosc) && (pole[2]->aktualna_wartosc == pole[6]->aktualna_wartosc))
    {
      if (pole[4]->aktualna_wartosc  == Pole::KOLKO)
        return WYGRYWA_KOLKO;
      if (pole[4]->aktualna_wartosc  == Pole::KRZYZYK)
        return WYGRYWA_KRZYZYK;
    }
    //sprawdzanie możliwości wykonania ruchu

    for (int i=0; i<9; i++)
      if (pole[i]->aktualna_wartosc == Pole::BRAK)
        return NIEROZSTRZYGNIETA;

    return REMIS;
}

void Gra::zmien_gracza()
{
    if (aktualny_gracz == G_KOLKO) {
        aktualny_gracz = G_KRZYZYK;
    } else {
        aktualny_gracz = G_KOLKO;
    }
}

void Gra::czysc_plansze()
{
    for (int i=0; i<9; i++) {
        pole[i]->resetuj();
    }
}

Gra::~Gra()
{
}


QByteArray Gra::IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

void Gra::wybranoPole(int i)
{
//    /*
//    char bufor;
//    struct wiadomosc *mojaWiad = (struct wiadomosc *) bufor;
//    mojaWiad->type = WIAD_TEKST;
//    strcpy(mojaWiad->dane.wiadomosc.napis, "Hello World");
//    mojaWiad->len = strlen(mojaWiad->dane.wiadomosc.napis)+1+3;
//    */
//    char text[80];
//    strcpy(text,"Hello World!");
//    quint8 size = sizeof(text);

//    QByteArray data;
//    QDataStream in(&data, QIODevice::WriteOnly);
//    in << WIAD_TEKST;
//    //in << size;
//    in << text;

//    if(socket->state() == QAbstractSocket::ConnectedState)
//    {
//        socket->write(IntToArray(data.size())); //write size of data
//        socket->write(data); //write the data itself
//        socket->waitForBytesWritten();
//    }

    char bajty[120];

    struct wiadomosc *tmp = (struct wiadomosc *) bajty;
    tmp->type = 25;
    tmp->dane.ruch.number = 3;
    tmp->len = 4;

//    char input = (char)i+1;
    //socket->write((char *) tmp, tmp->len);
    socket->write(bajty, tmp->len);



    socket->waitForReadyRead(3000);
    QByteArray data = socket->readAll();
    if (data.at(0)==-1) {
        QMessageBox::information(this,"Koniec gry", "Koniec gry");
    }

    qDebug() << data;
    int numer;
    for (int i=0; i<9; i++) {
        numer = int(data.at(i))-48;
        pole[i]->zaznacz_ruch((Pole::ruch_t)numer);
    }
}
