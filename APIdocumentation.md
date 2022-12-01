# Server endpoint documentation

### **Get requests**

---
* `getAllMics`  

    * **Description**  
    Le client demande la liste de tous les micros ainsi que des données disponibles à leurs sujet.  

    * **Response**  
    ```json
    {
        "statusCode": "integer",
        "message": "string",
        "length": "integer",
        "mics": [
            {
                "name": "string",
                "level": "integer",
                "isActive": "boolean",
            },
        ]
    }
    ```

---
* `getActions`  

    * **Description**  
    Récupération des données de toutes les actions enregistrées au niveau du serveur.  

    * **Response**
    ```json
    {
        "statusCode": "integer",
        "message": "string",
        "length": "integer",
        "actions": [
            "actionData", // @see actionData type below
        ]
    }
    ```

    Une action est constituée de 3 éléments:  
        - un nom (`string`)  
        - un type (un `integer` correspondant au type de l'action)  
        - un paramètre (`integer`)

    * `actionData`
    ```json
    {
        "name": "string",
        "type": "integer",
        "parameter": "integer"
    }
    ```

---
* `getWordDetections` (?)  

    * **Description**  
    Récupération des couples *[détection de mots]* / `callback`  

    * **Response**  
    ```json
    {
        "length": "integer",
        "data": [
            {
                "words": ["word1", "word2", "...", ],
                "action": "actionData",
            },
            {
                "...": "..."
            }
        ]
    }
    ```


### **Set requests**

---
* `setAutoAudioLeveler`  

    * **Description**  
    Activation / désactivation la gestion de volume automatique sur une entrée audio.

    * **Request**
    ```json
    {
        "enable": "boolean",
        "source": "string" // Nom de l'entrée audio (voir la requête getAllMics)
    }
    ```

    * **Response**
    ```json
    {
        "statusCode": "integer",
        "message": "string",
    }
    ```

---
* `setMicLevel`  

    * **Description**  
    Mise à jour de la valeur d'entrée d’un microphone.  

    * **Request**  
    ```json
    {
        "name": "string",
        "level": "integer",
        "setActive": "boolean",
    },
    ```

    * **Response**  
    ```json
    {
        "statusCode": "integer",
        "message": "string",
    }
    ```

___
* `setTranscriptSubscription`  

    * **Description**  
    Activer / Désactiver l'abonnement à la transcription de l'entrée d'un microphone spécifique.  
    Le paramètre `language` n'est pris en compte que si la transciption est activée.  

    * **Request**
    ```json
    {
        "enable": "boolean",
        "language": "string", // IETF language tag
    }
    ```

    * **Response**
    ```json
    {
        "statusCode": "integer",
        "message": "string",
        "subscriptionId": "integer"
    }
    ```

    Lorsque les sous-titres sont activés, le serveur enverras des paquets contenant la transcription en temps réel au client.  

    * **Transcript**
    ```json
    {
        "subscriptionId": "integer",
        "startTime": "integer",
        "duration": "integer",
        "transcript": "string"
    }
    ```

---
* `setWordDetection`  

    * **Description**  
    Mise en place d'un `callback` lors de la détection d'un ou de plusieurs mot(s) / phrase(s).
    Mise à jour des word detection (notamment pour les suppressions)

    * **Request**
    ```json
    {
        "words": [
            "word1",
            "word2",
            "word3",
        ],
        "callback": "actionData",
    }
    ```

    * **Response**
    ```json
    {
        "statusCode": "integer",
        "message": "string"
    }
    ```
