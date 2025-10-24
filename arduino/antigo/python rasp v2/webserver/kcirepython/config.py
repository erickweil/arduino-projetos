FAKECONNECTION = False
COMPORT = "/dev/ttyUSB0"
BAUDRATE = 9600

PAGE_URL = "http://localhost:8081/index.html"
# isso aceita qualquer host, permitindo assim acessar pelo
# localhost, 127.0.0.1, ip do modem ( caso haja redirecionamento de porta)
# e ate por um host DDNS, tambem, se houver redirecionamento de porta
HOST = "0.0.0.0"
# para evitar conflitos, uma porta pouco usada
WEBPORT = 8081

# motor de passo
PASSOS_POR_VOLTA = 200
PASSOS_180 = 100
PASSOS_90 = 50
PASSOS_45 = 25
PASSOS_9 = 5
PASSOS_1_8 = 1

# o que segue, e totalmente arbitrario, podem ser usadas quaisquer letras ASCII
# desde que o arduino converse na mesma 'lingua' e contenha apenas 1 caractere

# comunicação teste
COD_PING = '@'.encode()

# controle motor de passo ( que sobe e desce a prateleira )
COD_SETAR_POSICAO = 'a'.encode()
COD_LER_POSICAO = 'b'.encode()
COD_LER_ALVO = 'c'.encode()
COD_PARAR_MOTOR = 'd'.encode()

# controle motor servo ( que gira a amostra)
COD_SETAR_GIROVEL = 'e'.encode()
COD_LER_GIROVEL = 'f'.encode()

# calibração
COD_CALIBRA_MIN = '0'.encode()
COD_CALIBRA_MAX = '1'.encode()
COD_SETAR_PASSOS = '2'.encode()
COD_ADD_PASSOS = '3'.encode()
COD_SALVAR_CALIBRA = '4'.encode()
COD_CALIBRA_MOTOR = '5'.encode()

# para n ter erro
COD_ESPACO = ' '.encode()
COD_FIM = '\n'.encode()
COD_OK = "OK".encode()

# caminho das imagens da interface
IMG = {}
IMG["sign_warning"] = "icones/sign_warning.gif"
IMG["ok"] = "icones/ok.gif"
IMG["kcire"] = "icones/kcire.gif"
IMG["icone"] = "icones/icone.ico"
