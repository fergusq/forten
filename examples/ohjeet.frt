\Include{examples/rnl.frt}

Tämä asiakirja on tarkoitettu yhtä aikaa sekä
esimerkiksi että ohjeeksi. Se havainnollistaa
rnl.frt:n ominaisuuksia.

\Section{JOHDANTO}

Forten on kevyt esikääntäjä, joka ei itsessään
tee juuri mitään. Jotta siitä olisi hyötyä, on
käyttäjän määriteltävä haluamansa muotoilut
Fortenin tarjoamilla työkaluilla. Nämä työkalut
eivät ole kovin helppokäyttöisiä: oikeastaan
Fortenin syntaksi on hirvittävää. Tästä syystä
yksinkertaisinta onkin käyttää jotain valmista
kirjastoa.

rnl.frt on yksinkertainen esimerkkikirjasto,
joka tunnistaa kappalejaot ja muotoilee
otsikot.

\Section{RNL.FRT:N OMINAISUUDET}

\SubSection{Kappalejaot}

rnl.frt pitää tyhjää riviä kappalejakona ja
säilyttää sen myös lopullisessa asiakirjassa.
Muut rivinvaihdot kirjasto poistaa.

\SubSection{Otsikot}

rnl.frt numeroi jokaisen otsikon, mikä
helpottanee asiakirjan lukemista.

\Section{RNL.FRT:N KÄYTTÄMINEN}

\SubSection{rnl.frt:n sisällyttäminen asiakirjaan}

rnl.frt sisällytetään asiakirjaan Include-komennolla:

```\Include{examples/rnl.frt}

\SubSection{Otsikoiden luominen}

Yläotsikko määritellään Section-komennolla ja
alaotsikko SubSection-komennolla:

```\Section{OTSIKKO}

```\SubSection{ALAOTSIKKO}

\Section{LOPUKSI}

Toivon, että rnl.frt auttaa käyttäjiä
luomaan omia kirjastojaan, jotka ovat
toivottaasti parempia kuin tämä pieni
esimerkki.
