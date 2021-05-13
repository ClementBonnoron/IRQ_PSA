compiler le module :
```c
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
```

Ajouter le module au noyau :
```c
sudo insmod irq_timestamp.ko time_before_logout=...
```
Le module contient un argument spécifiant la durée après laquelle un utilisateur
est considéré comme ayant été déconnecté après une durée d'inactivitée.