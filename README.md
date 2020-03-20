# WebServX

WebServX est une serveur Web minimaliste réalisé dans le cadre du cours M4101C (Programmation système avancé).


## Pour commencer

Ces instructions vous permettront d'obtenir une copie du projet sur votre ordinateur local à des fins de développement et de test.

### Installation

Commencez par cloner le projet sur votre machine locale

```
git clone https://gitlab.univ-lille.fr/mathys.pomier.etu/webservx.git
```

Compilez le projet

```
make -B
```
### Utilisation

Lancez le serveur web

```
cd webserver/
```
```
./webservx <racine de votre site>
```
Nous vous mettons à disposition un site minimaliste pour vos test

```
./webservx ../simple_website/
```

Une fois ceci terminé vous pouvez effectuer des requêtes au serveur.

* Soit avec la commande `nc` :

```
nc localhost 8080
GET / HTTP/1.1

HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 609

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Page d'accueil</title>
</head>
<body>
    <h1>Page d'accueil</h1>
    <p>Lorem ipsum dolor sit amet consectetur adipisicing elit. Nulla, veritatis dolore velit libero quae eaque aliquam. Excepturi id unde cumque placeat quia molestiae eius harum quasi exercitationem atque. Quia, amet.</p>
    <ul>
        <li><a href="page2.html">Direction la page n°2</a></li>
    </ul>

    <img src="test_image.png" alt="" width="500">
</body>
</html>
```

* Soit via un `navigateur` :

![Image d'exemple de requête](https://mathys-pomier.fr/images/image_exemple_webservx.png)

Vous pouvez également accéder au statistiques du serveur pour cela vous devez éffectuer une requête sur `/stats` :
```
nc localhost 8080
GET /stats HTTP/1.1

HTTP/1.1 200 OK
Content-Length: 500
Content-Type: text/html

Stats :
        Nombre de connexions : 4
        Nombre de requêtes : 6
        Nombre de code de retour 200 : 4
        Nombre d'erreurs 400 : 1
        Nombre d'erreurs 403 : 0
        Nombre d'erreurs 404 : 1
        Nombre d'erreurs 405 : 0
```

## Auteurs

* **Mathys POMIER** - [Elkios](https://github.com/Elkios)
* **Lucas ANTOINE** - [Elkios](https://github.com/Elkios)

