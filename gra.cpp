#include "gra.h"

Gra::Gra(QWidget *parent) :
    QMainWindow(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(disconnected()),this,SLOT(rozlacz()));
    connect(socket,SIGNAL(readyRead()),SLOT(czytajDane()));

    QWidget *mainwidget = new QWidget(this);

    setWindowTitle("Kółko i krzyżyk");
    QWidget *plansza = new QWidget(this);

    QHBoxLayout *mainlayout = new QHBoxLayout;

    //Siatka
    QGridLayout *siatka = new QGridLayout;

    int col=0;
    pole = new Pole*[9];
    for (int i=0; i<9; i++) {
        pole[i] = new Pole(i);
        connect(pole[i],SIGNAL(wybrano(int)),this,SLOT(wybranoPole(int)));
        siatka->addWidget(pole[i],i%3,col);
        if (i%3==2) col++;
    }
    address = new QLineEdit(this);
    address->setText("127.0.0.1");
    siatka->addWidget(address,4,0,1,2);

    port = new QLineEdit(this);
    port->setText("2222");
    port->setMaximumSize(50,100);
    siatka->addWidget(port,4,2);

    connectButton = new QPushButton("Połącz", this);
    siatka->addWidget(connectButton,5,0,3,3);
    connect(connectButton,SIGNAL(clicked(bool)),this,SLOT(przyciskClicked()));

    rezultat = NIEROZSTRZYGNIETA;
    aktualny_gracz = G_KOLKO;

    //Czat
    QVBoxLayout *czatbox = new QVBoxLayout;
    czatout = new QTextEdit;
    czatout->setReadOnly(true);
    czatbox->addWidget(czatout);

    czatin = new QLineEdit;
    czatbox->addWidget(czatin);
    connect(czatin,SIGNAL(returnPressed()),this,SLOT(wyslijTekst()));

    mainlayout->addLayout(siatka);
    mainlayout->addLayout(czatbox);

    plansza->setLayout(siatka);
    mainwidget->setLayout(mainlayout);

    setCentralWidget(mainwidget);
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

void Gra::przyciskClicked()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        poddajSie();
        socket->abort();
    } else {
        polacz();
    }
}

void Gra::polacz()
{
    QHostAddress hostip(address->text());
    quint16 hostport = port->text().toShort();
    if (hostip.isNull()!=true && hostport>0 && hostport<10000) {
        socket->connectToHost(hostip,hostport);
        if (socket->waitForConnected()) {
            qDebug() << "Połączono z serwerem";
            connectButton->setText("Rozłącz");
            address->setDisabled(true);
            port->setDisabled(true);
            dodajCzat(INFO,QString("Połączono z serwerem ").append(address->text()));
        } else {
            qDebug() << "Nie udało się połączyć z serwerem";
        }
    }
}

void Gra::rozlacz()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->abort();
    }
    qDebug() << "Rozłączono z serwerem";
    czysc_plansze();
    connectButton->setText("Połącz");
    address->setDisabled(false);
    port->setDisabled(false);
    dodajCzat(INFO,"Zakończono połączenie");
}

void Gra::czytajDane()
{
    QByteArray dane = socket->read(120);
    //odczytałem dane.length();
    const char *tmp = dane.constData();

    struct Gra::wiadomosc *wiad = (struct Gra::wiadomosc *) tmp; //dane.constData();

    //TODO
    QString oldinfo;
    QString newinfo;
    plansza_t mojaPolansza;

    switch (wiad->type) {
    case Gra::WIAD_TEKST:
        dodajCzat(SERWER,wiad->dane.czat.napis);
        break;
    case Gra::PLANSZA:
        mojaPolansza = wiad->dane.mojaMapa.plansza;
        for (int i=0; i<9; i++) {
            pole[i]->zaznacz_ruch((Pole::ruch_t)mojaPolansza.ruchy[i]);
        }
        break;
    case Gra::POTWIERDZENIE:
//        if ((rezultat_t)wiad->dane.potwierdzenie.rezultat != REMIS) {
//            koniecGry((rezultat_t)wiad->dane.potwierdzenie.rezultat);
//        }
        break;
    default:

        break;
    }

//    if (data.at(0)==-1) {
//        QMessageBox::information(this,"Koniec gry", "Koniec gry");
//    }
}

void Gra::wyslijTekst()
{
    if (socket->state() == QAbstractSocket::ConnectedState && czatin->text().isEmpty()==false) {
        QByteArray ba = czatin->text().toLatin1();
        const char *tekst = ba.data();
        char bajty[120];
        struct wiadomosc *tmp = (struct wiadomosc *) bajty;
        strcpy(tmp->dane.czat.napis,tekst);
        tmp->type = Gra::WIAD_TEKST;
        tmp->length = sizeof(bajty);
        socket->write(bajty, tmp->length);
        dodajCzat(KLIENT,czatin->text());
        czatin->setText("");
    }
}


QByteArray Gra::IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

void Gra::poddajSie()
{
    char bajty[120];
    struct wiadomosc *wiad = (struct wiadomosc*) bajty;
    wiad->dane.poddanie.poddalSie = 1;
    wiad->length = sizeof(bajty);
    socket->write(bajty,wiad->length);
    dodajCzat(INFO, "Poddałeś się.");
    qDebug() << "Poddałem sie";
}

void Gra::dodajCzat(Gra::czat_t rodzaj, QString tekst)
{
    QString oldinfo = czatout->toHtml();
    QString newinfo;
    switch (rodzaj) {
    case Gra::KLIENT:
        newinfo = "<b>Klient</b>: ";
        break;
    case Gra::SERWER:
        newinfo = "<b>Serwer</b>: ";
        break;
    default:
        newinfo = "<font color='blue'>Info</font>: ";
        tekst.prepend("<i>");
        tekst.append("</i>");
        break;
    }
    newinfo.append(tekst);
    czatout->setHtml(oldinfo.append(newinfo));

    //TODO - To nie działa
    czatout->setReadOnly(false);
    czatout->ensureCursorVisible();
    czatout->verticalScrollBar()->setValue(czatout->verticalScrollBar()->maximum());
    czatout->setReadOnly(true);
}

void Gra::koniecGry(Gra::rezultat_t wynik)
{
    for (int i=0; i<9; i++) {
        pole[i]->resetuj();
        dodajCzat(INFO,"Koniec gry");
    }
}

void Gra::wybranoPole(int i)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        char bajty[120];
        struct wiadomosc *tmp = (struct wiadomosc *) bajty;
        tmp->type = Gra::RUCH;
        tmp->dane.ruch.x = i/3;
        tmp->dane.ruch.y = i%3;
        tmp->length = sizeof(bajty); //4
        socket->write(bajty, tmp->length);
    }
}
