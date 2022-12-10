# Server endpoint documentation


## **Get requests**
<br>

### **Getting all microphones data**

* **Description**  
Le client demande la liste de tous les micros ainsi que des données disponibles à leurs sujet.  

* **Request**  
```json
{
    "command": "getAllMics",
}
```

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

<br>

---
### **Getting action / reaction couples**

* **Description**  
Récupération des données de toutes les actions enregistrées au niveau du serveur.  

* **Request**
```json
{
    "command": "getActReactCouples",
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
    "length": "integer",
    "actReacts": [
        {
            "actReactId": "integer",
            "isActive": "boolean",
            "action": {
                "actionId": "integer",
                "type": "action_type",
                "params": {
                    "..." // Depends on action type
                }
            },
            "reaction": {
                "reactionId": "integer",
                "type": "reaction_type",
                "params": {
                    "..." // Depends on reaction type
                }
            }
        },
        "..." // Next element
    ]
}
```


<br>

## **Set requests**

### **Setting automatic audio leveler** - *Audio compressor*
* **Description**  
Activation / désactivation la gestion de volume automatique sur une entrée audio.

* **Request**
```json
{
    "command": "setAutoAudioLeveler",
    "params": {
        "enable": "boolean",
        "source": "string", // Nom de l'entrée audio (voir la requête getAllMics)
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>  

---
### **Setting microphones input level**
* **Description**  
Mise à jour de la valeur d'entrée de microphones.  

* **Request**  
```json
{
    "command": "setMicLevel",
    "params": {
        "mics": [
            {
                "name": "string",
                "level": "integer",
                "setActive": "boolean",
            },
            "..."
        ]
    }
},
```

* **Response**  
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>

---
### **Enabling / Disabling subtitles**
* **Description**  
Activer / Désactiver les sous-titres transcrit de l'entrée d'un microphone spécifique.  
Le paramètre `language` n'est pris en compte que si la transciption est activée.  

* **Request**
```json
{
    "command": "setSubscription",
    "params": {
        "enable": "boolean",
        "language": "string", // IETF language tag
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>

---
### **Setting an action / reaction couple**  

* **Description**  
Mise en place d'un couple action/reaction au niveau du plugin.  
Lorsque l'action est détectée, la réaction est déclenchée automatiquement.  

* **Request**
```json
{
    "command": "setActionReaction",
    "params": {
        "action": {
            "type": "action_type",
            "params": {
                "..."
            }
        },
        "reaction": {
            "type": "reaction_type",
            "params": {
                "..."
            }
        }
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
    "data": {
        "actionId": "integer",
        "reactionId": "integer",
        "actReactId": "integer",
    }
}
```

<br>

## **Remove and Update requests**

### **Remove an action / reaction couple**  

* **Description**  
Suppression d'un couple action/reaction au niveau du plugin.  

* **Request**
```json
{
    "command": "removeActReact",
    "params": {
        "actReactId": "integer",
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>

---
### **Updating an action**  

* **Description**  
Mise à jour d'une action côté plugin.  
L'action peut changer de type et / ou de paramètres.  
Une fois mise à jour, l'action reste reliée à sa/ses réaction(s).  
Si la requête est invalide, l'action reste inchangée.

* **Request**
```json
{
    "command": "updateAction",
    "params": {
        "actionId": "integer",
        "type": "action_type",
        "params": {
            "..."
        }
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>

---
### **Update reaction**  

* **Description**  
Mise à jour d'une réaction côté plugin.  
La réaction peut changer de type et / ou de paramètres.  
Une fois mise à jour, la réaction reste reliée à son/ses action(s).  
Si la requête est invalide, l'action reste inchangée.

* **Request**
```json
{
    "command": "updateReaction",
    "params": {
        "reactionId": "integer",
        "type": "reaction_type",
        "params": {
            "..."
        }
    }
}
```

* **Response**
```json
{
    "statusCode": "integer",
    "message": "string",
}
```

<br>
<br>

# **Actions / Reactions**

## *Actions*

**Word detection**
```json
{
    "type": "WORD_DETECT",
    "params": {
        "words": ["word1", "word2", "..."]
    },
}
```

**Application launch**
```json
{
    "type": "APP_LAUNCH",
    "params": {
        "app_name": "app_identifier"
    },
}
```

**Key pressed**
```json
{
    "type": "KEY_PRESSED",
    "params": {
        "key": "key_identifier"
    },
}
```

---
## *Reactions*

**Scene switch**
```json
{
    "type": "SCENE_SWITCH",
    "params": {
        "scene": "scene_identifier"
    }
}
```

**Toggle audio compressor**
```json
{
    "type": "TOGGLE_AUDIO_COMPRESSOR",
    "params": {
        "audio-source": "audio_source_identifier"
    }
}
```