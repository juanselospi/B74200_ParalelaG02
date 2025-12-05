✅ 1. Instalar OpenVPN

En Fedora:

sudo dnf install openvpn -y


En Ubuntu/Debian:

sudo apt install openvpn -y

✅ 2. Ir a tu carpeta donde guardaste la VPN

Tú dijiste que están en:

/home/juanselospi/Desktop/UCR/VPN UCR


Primero muévete ahí:

cd "/home/juanselospi/Desktop/UCR/VPN UCR"


(OJO: como el nombre tiene espacio, necesitas comillas).

✅ 3. Verifica que tus archivos estén ahí
ls -l


Deberías ver algo como:

ecci.conf 

ecci

ecci.crt

ecci.key

ecci-ca.crt

ecci-ta.key

✅ 4. Habilitar la clave TLS (recomendado)

En tu archivo ecci.conf, la línea del tls-auth está comentada:

#tls-auth ecci-ta.key 1


Para evitar problemas, edítalo y descoméntalo:

nano ecci.conf


→ Cambia:

#tls-auth ecci-ta.key 1


→ A:

tls-auth ecci-ta.key 1


Guarda con CTRL+O, Enter, y sal con CTRL+X.

✅ 5. Conectarte manualmente a la VPN

Ahora prueba conectarte:

sudo openvpn --config ecci.conf


El sistema te pedirá:

Enter Auth Username:
Enter Auth Password:


Usuario: tu correo UCR
Contraseña: tu contraseña de correo/UCR

Si todo funciona, verás algo como:

Initialization Sequence Completed


Y ya podrás hacer SSH a poas:

ssh B74200@poas.ecci.ucr.ac.cr

✅ 6. (Opcional) Conectar la VPN en segundo plano

Si quieres que se ejecute sin llenar la terminal:

sudo openvpn --config ecci.conf --daemon


Para ver si está conectado:

ip a | grep tun


Para desconectar:

sudo killall openvpn

🧪 7. Probar conexión al cluster “poas”

Cuando la VPN esté activa:

ssh tu_usuario@163.178.104.68

✅ 1. Conéctate a la VPN

Desde tu carpeta donde está ecci.conf:

cd "/home/juanselospi/Desktop/UCR/VPN UCR"
sudo openvpn --config ecci.conf


Cuando veas:

Initialization Sequence Completed


ya estás dentro de la red de la UCR.

✅ 2. Conectarte a poas por SSH

Con soporte gráfico (-Y):

ssh -Y B74200@10.84.19.15


✔ -Y habilita X11 forwarding, por si querés abrir cosas gráficas como gedit, evince, etc.

Si te sale la primera vez:

Are you sure you want to continue connecting? (yes/no)


Escribí:

yes


Y listo, estarás dentro de POÁS:

[B74200@poas ~]$

🔍 Consejo importante

Si el comando se queda pegado en:

ssh: connect to host 10.84.19.15 port 22: No route to host


→ No estás conectado a la VPN.

Si sale:

ssh: connect to host 10.84.19.15 port 22: Connection timed out


→ La VPN está mal conectada o el archivo .conf tiene algo comentado que no debe.


# COPIAR ARCHIVOS AL CLUSTER

📌 ¿Cómo copiar mi carpeta examen2 al cluster poas desde otra terminal?

No importa si tienes una terminal conectada al VPN y otra conectada al cluster. Desde una tercera terminal puedes copiar archivos usando scp, siempre que la máquina donde corres scp esté conectada al VPN.

✅ Comando general
scp -r /ruta/local/examen2 B74200@10.84.19.15:/ruta/remota/

📍 Ejemplo típico si estás en tu PC (con VPN activa)

Supongamos que tu carpeta está aquí:

~/Desktop/UCR/Paralela/examen2


La envías al cluster así:

scp -r ~/Desktop/UCR/Paralela/examen2 B74200@10.84.19.15:~/


Esto copia la carpeta examen2 al home de tu usuario dentro de poas.

🔍 Notas importantes

Debes tener la VPN encendida en la terminal desde la que corres scp.

No tienes que estar dentro del cluster para copiar cosas; scp funciona desde tu máquina local.

-r significa recursive, necesario para copiar carpetas completas.

Puedes copiar hacia dentro o hacia afuera del cluster. Por ejemplo, para traer cosas del cluster:

scp -r B74200@10.84.19.15:~/examen2 ~/Desktop/
