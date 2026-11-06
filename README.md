# 💬 ft_irc — Kendi IRC Sunucunu Yaz

<div align="center">

![42 School](https://img.shields.io/badge/School-42-black?style=for-the-badge&logo=42)
![C++98](https://img.shields.io/badge/C++-98-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Network](https://img.shields.io/badge/Network-TCP%2FIP-blue?style=for-the-badge)
![poll](https://img.shields.io/badge/I%2FO-poll()-orange?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)

**C++98 ile sıfırdan yazılmış, `poll()` tabanlı, tek thread'li bir IRC sunucusu**

*Non-blocking socket'lar, komut ayrıştırma, kanal yönetimi, operatör modları ve bonus olarak JokeBot.*

[Kurulum](#-kurulum) • [Kullanım](#-kullanım) • [Özellikler](#-desteklenen-özellikler) • [Mimari](#-mimari) • [Test](#-test-senaryoları)

</div>

---

## 🎯 Projenin Amacı


<img align="right" alt="Raycasting Demo" width="320" src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExNGhrNDlscHF2Znpnc2xkaHdnMDNsdWJnazQ4ankwbmg1aWZsenRqMyZlcD12MV9naWZzX3NlYXJjaCZjdD1n/kqGp0mQGvdsg2MEiEr/giphy.gif">

**ft_irc**, gerçek bir IRC (Internet Relay Chat) istemcisinin (HexChat, irssi, WeeChat…) bağlanıp sohbet edebildiği bir **IRC sunucusu** yazmayı amaçlar. Proje; TCP soket programlama, olay tabanlı I/O ve metin tabanlı bir protokolün ayrıştırılması konularını derinlemesine öğretir.

### 🎓 **Ana Öğrenme Hedefleri**

#### 🔌 **Soket Programlama**
- `getaddrinfo`, `socket`, `setsockopt`, `bind`, `listen`, `accept` akışını kurma
- `SO_REUSEADDR` ile portun hızlıca yeniden kullanılabilmesini sağlama
- Tüm fd'leri `fcntl(F_SETFL, O_NONBLOCK)` ile **non-blocking** yapma

#### 📡 **Olay Tabanlı I/O (`poll`)**
- Tek bir `poll()` döngüsüyle tüm istemcileri aynı anda yönetme (thread / fork yok)
- `POLLIN` ile okuma, `POLLOUT` ile yalnızca gönderilecek veri varken yazma
- İstemci başına **okuma / yazma tamponu** tutarak kısmi (fragmented) paketleri doğru işleme

#### 📝 **Protokol Ayrıştırma**
- `\r\n` (ve yalnızca `\n`) ile biten satırları ayırma
- `COMMAND arg1 arg2 :trailing parametre` formatını `Command` yapısına dönüştürme
- Komutları büyük/küçük harf duyarsız eşleştirme

#### 🏠 **Kanal ve Yetki Yönetimi**
- Kanal oluşturma, üyelik, operatör (`@`) ve davet listeleri
- `i`, `t`, `k`, `o`, `l` kanal modlarının uygulanması
- Standart IRC sayısal cevap kodlarıyla (numeric reply) hata bildirimi

#### 🧹 **Güvenli Kapanış**
- `SIGINT` / `SIGQUIT` ile temiz kapanış, `SIGPIPE` yok sayma
- İstemci koptuğunda kanallardan, davet ve operatör listelerinden otomatik temizlik

---

## ✨ Desteklenen Özellikler

<div align="center">

![Channels](https://img.shields.io/badge/Channels-Supported-brightgreen?style=for-the-badge)
![Modes](https://img.shields.io/badge/Modes-itkol-blue?style=for-the-badge)
![Bot](https://img.shields.io/badge/JokeBot-Bonus-orange?style=for-the-badge)

</div>

### 📋 **Mandatory Özellikler**

| Komut | Örnek | Açıklama |
|-------|-------|----------|
| **PASS** | `PASS sifre` | Sunucu şifresi ile kimlik doğrulama |
| **NICK** | `NICK sude` | Takma ad belirleme / değiştirme (benzersiz olmalı) |
| **USER** | `USER sude 0 * :Sude Naz` | Kullanıcı kaydını tamamlar (en az 4 parametre) |
| **JOIN** | `JOIN #genel,#42 anahtar` | Bir veya birden fazla kanala katılma (virgülle ayrılmış) |
| **PART** | `PART #genel :görüşürüz` | Kanaldan ayrılma |
| **PRIVMSG** | `PRIVMSG #genel :selam` | Kanala veya kullanıcıya mesaj |
| **KICK** | `KICK #genel ahmet :spam` | Kullanıcıyı kanaldan atma *(operatör)* |
| **INVITE** | `INVITE ahmet #genel` | Kullanıcıyı kanala davet etme *(operatör)* |
| **TOPIC** | `TOPIC #genel :yeni konu` | Konuyu görüntüle / değiştir |
| **MODE** | `MODE #genel +it` | Kanal modlarını görüntüle / değiştir *(operatör)* |
| **PING** | `PING token` | `PONG` ile cevaplanır |
| **QUIT** | `QUIT :bye` | Bağlantıyı sonlandırır, ortak kanallara haber verir |

### 🔧 **Kanal Modları**

| Mod | Parametre | Açıklama |
|-----|-----------|----------|
| `+i` / `-i` | — | **Invite-only**: yalnızca davet edilenler katılabilir |
| `+t` / `-t` | — | **Topic kısıtlaması**: konuyu yalnızca operatörler değiştirebilir |
| `+k` / `-k` | `<anahtar>` | Kanal şifresi koyar / kaldırır |
| `+o` / `-o` | `<nick>` | Operatör yetkisi verir / alır |
| `+l` / `-l` | `<limit>` | Kullanıcı sınırı koyar / kaldırır |

> Birden fazla mod tek satırda verilebilir: `MODE #genel +itk anahtar`, `MODE #genel +ol ahmet 10`

### 🎁 **Bonus: JokeBot 🤖**

| Özellik | Açıklama |
|---------|----------|
| **Kanalda tetikleme** | Bir kanala `!joke` veya `!espri` yazılırsa JokeBot rastgele bir espri atar |
| **Özel mesaj** | `PRIVMSG JokeBot :merhaba` yazan kullanıcıya doğrudan espri gönderir |
| **Otomatik espri** | Her **5 dakikada** bir (300 sn) aktif tüm kanallara espri yayınlar |

---

## 📁 Dosya Yapısı

```
ft_irc/
│
├── 📄 Makefile
├── 📄 README.md
│
├── 🗂️ includes/
│   ├── Server.hpp          # Server sınıfı (poll döngüsü, istemci yönetimi)
│   ├── Client.hpp          # Client sınıfı (fd, nick, okuma/yazma tamponu)
│   ├── Command.hpp         # Command struct + handleXxx prototipleri
│   ├── Parser.hpp          # Ham satır → Command
│   ├── Channel.hpp         # ClientInfo, ChannelInfo, global durum tabloları
│   ├── Helpers.hpp         # require* doğrulayıcıları, broadcast yardımcıları
│   └── Bot.hpp             # JokeBot espri listesi
│
└── 🧩 srcs/
    ├── main.cpp            # Argüman kontrolü, sinyaller, Server başlatma
    ├── Server.cpp          # init / run / accept / read / write / disconnect
    ├── Client.cpp          # Tampon yönetimi
    ├── Parser.cpp          # IRC satır ayrıştırıcı
    ├── Commands.cpp        # Server::executeCommand — komut yönlendirici
    ├── CmdUtils.cpp        # Global tablolar, isOp / isMember / userPrefix …
    ├── Bot.cpp             # Periyodik JokeBot yayını
    └── cmds/
        ├── Helpers.cpp     # Ortak doğrulama ve yayın fonksiyonları
        ├── Pass.cpp  Nick.cpp  User.cpp
        ├── Join.cpp  Part.cpp  Quit.cpp  Privmsg.cpp
        └── Kick.cpp  Invite.cpp  Topic.cpp  Mode.cpp
```

### 📊 **Dosya Açıklamaları**

| Dosya | Sorumluluk |
|-------|------------|
| `Server.cpp` | Soket kurulumu, `poll()` ana döngüsü, bağlantı kabul/kopma, tamponlu okuma-yazma |
| `Parser.cpp` | `raw_line` → `Command { command, args, raw }`; `:` ile başlayan trailing parametreyi tek argüman yapar |
| `Commands.cpp` | Komut adını büyük harfe çevirir ve ilgili `handleXxx` fonksiyonuna yönlendirir |
| `CmdUtils.cpp` | `s_clients` / `s_channels` tabloları, `cleanupZombieFD` ile kopan istemci temizliği |
| `cmds/Helpers.cpp` | `requireRegistered`, `requireOp`, `broadcastToChannel`, `leaveChannel`, `sendWelcomeBurst` … |
| `cmds/*.cpp` | Her IRC komutu için tek bir `handleXxx` fonksiyonu |
| `Bot.cpp` | Zamanlayıcı ile tüm kanallara espri yayını |

---

## 🚀 Kurulum

### 📋 **Ön Gereksinimler**

![C++](https://img.shields.io/badge/c%2B%2B%20(C%2B%2B98)-Required-red?style=flat-square)
![Make](https://img.shields.io/badge/Make-Required-blue?style=flat-square)

- **Derleyici**: `c++` (g++ veya clang++), `-std=c++98` desteği
- **Unix-like OS**: Linux veya macOS
- Harici kütüphane **gerekmez** — yalnızca standart C++98 ve POSIX soket API'leri

### 📥 **Klonlama ve Derleme**

```bash
git clone https://github.com/skarayil/ft_irc.git
cd ft_irc

make        # ircserv derler
make clean  # Object dosyalarını siler
make fclean # Object + executable siler
make re     # Baştan derler
```

Derleme bayrakları: `-Wall -Werror -Wextra -std=c++98`

Çıktı: kök dizinde **`ircserv`** çalıştırılabilir dosyası.

---

## 💻 Kullanım

### 🖥️ **Sunucuyu Başlatma**

```bash
./ircserv <port> <password>
```

```bash
./ircserv 6667 sifre123
```

| Parametre | Kural |
|-----------|-------|
| `port` | Yalnızca rakam, **1024 – 65535** aralığında |
| `password` | Boş olamaz; istemciler `PASS` ile bu şifreyi göndermelidir |

Hatalı kullanımda program mesaj basıp `1` ile çıkar:

```bash
$ ./ircserv
Usage: ./ircserv <port> <password>

$ ./ircserv 80 sifre
Error: Wrong port parameters
```

### 🔗 **İstemci ile Bağlanma**

#### 🟢 **HexChat / irssi**

```text
Sunucu : localhost
Port   : 6667
Şifre  : sifre123
```

```bash
# irssi örneği
irssi -c localhost -p 6667 -w sifre123 -n sude
```

#### 🟡 **netcat ile elle test**

```bash
nc -C localhost 6667
```

```text
PASS sifre123
NICK sude
USER sude 0 * :Sude Naz
:ircserv 001 sude :Welcome to the IRC Network sude!
JOIN #genel
PRIVMSG #genel :Herkese merhaba!
```

> `-C` bayrağı satır sonlarını `\r\n` olarak gönderir. Sunucu yalnızca `\n` ile biten satırları da kabul eder.

### 📝 **Temel Örnekler**

#### 🟠 **Kanal İşlemleri**

```text
JOIN #genel                 # kanalı oluştur (ilk giren operatör olur)
JOIN #gizli anahtar         # +k'li kanala giriş
TOPIC #genel :Hoş geldiniz  # konu belirle
TOPIC #genel                # konuyu sorgula
PART #genel :çıkıyorum
```

#### 🔴 **Operatör Komutları**

```text
MODE #genel +i              # invite-only yap
INVITE ahmet #genel         # ahmet'i davet et
MODE #genel +o ahmet        # ahmet'i operatör yap
MODE #genel +k gizli        # şifre koy
MODE #genel +l 20           # en fazla 20 kullanıcı
KICK #genel mehmet :kurallara uy
```

#### 🟣 **JokeBot**

```text
PRIVMSG #genel :!joke       # kanalda espri iste
PRIVMSG JokeBot :selam      # özelden espri iste
```

---

## 🏗️ Mimari

### 🔄 **İstek İşleme Akışı**

```
        İstemci (TCP)
              │
              ▼
   ┌────────────────────┐
   │   poll() döngüsü   │  Server::run — tüm fd'leri tek yerden izler
   └─────────┬──────────┘
             │ POLLIN
             ▼
   ┌────────────────────┐
   │  handleClientRead  │  recv → Client::_readBuffer'a ekle
   │                    │  '\n' görene kadar satır satır ayır
   └─────────┬──────────┘
             ▼
   ┌────────────────────┐
   │  Parser::parse()   │  "JOIN #a,#b key" → Command{ JOIN, [#a,#b, key] }
   └─────────┬──────────┘
             ▼
   ┌────────────────────┐
   │   executeCommand   │  Komutu büyük harfe çevir, handleXxx'e yönlendir
   └─────────┬──────────┘
             ▼
   ┌────────────────────┐
   │   handleXxx(...)   │  require* kontrolleri → durum değişikliği
   └─────────┬──────────┘
             ▼
   ┌────────────────────┐
   │    sendReply()     │  Yazma tamponuna ekle + POLLOUT'u aç
   └─────────┬──────────┘
             │ POLLOUT
             ▼
   ┌────────────────────┐
   │ handleClientWrite  │  send → gönderileni tampondan sil
   └────────────────────┘  Tampon boşalınca POLLOUT kapanır
```

### 🔐 **Kayıt (Registration) Akışı**

```
PASS  →  authenticated = true
NICK  →  takma ad atanır           (PASS olmadan reddedilir: 451)
USER  →  username atanır           (en az 4 parametre)
         ┌─ NICK + USER tamam ─►  registered = true
         └─►  001 / 002 / 003 / 004  karşılama mesajları
```

`NICK` ve `USER` hangi sırayla gelirse gelsin, ikisi de tamamlandığında kayıt biter ve karşılama mesajı gönderilir. Kayıttan önce kanal komutları `451 You have not registered` ile reddedilir.

### 🧱 **Ana Veri Yapıları**

| Yapı | Alanlar | Amaç |
|------|---------|------|
| `Client` | `_fd`, `_nickname`, `_readBuffer`, `_writeBuffer` | Bağlantıya ait ağ durumu |
| `ClientInfo` | `username`, `authenticated`, `registered` | Protokol seviyesinde kayıt durumu |
| `ChannelInfo` | `members`, `operators`, `invited`, `topic`, `key`, `userLimit`, `inviteOnly`, `topicOps` | Kanal durumu |
| `Command` | `command`, `args`, `raw` | Ayrıştırılmış tek bir IRC satırı |
| `s_clients` | `std::map<int, ClientInfo>` | fd → kayıt bilgisi |
| `s_channels` | `std::map<std::string, ChannelInfo>` | kanal adı → kanal bilgisi |

### 🛡️ **Doğrulayıcı (Guard) Fonksiyonlar**

Her komut aynı küçük doğrulayıcıları zincirler; böylece hata mesajları tek yerde, tutarlı kalır:

```cpp
if (!requireRegistered(srv, fd, nick, prefix))          return ;
if (!requireMinArgs(srv, fd, cmd, 1, "TOPIC", nick, prefix)) return ;
if (!requireChannelExists(srv, fd, chan, nick, prefix)) return ;
if (!requireMember(srv, fd, chan, nick, prefix))        return ;
if (!requireOp(srv, fd, chan, nick, prefix))            return ;
```

### 📡 **Sinyal Yönetimi**

| Sinyal | Davranış |
|--------|----------|
| `SIGINT` (Ctrl+C) | `g_stop = true` → döngü biter, tüm fd'ler kapatılır |
| `SIGQUIT` (Ctrl+\\) | Aynı şekilde temiz kapanış |
| `SIGPIPE` | `SIG_IGN` — kopmuş istemciye yazarken sunucu çökmez |

### 📨 **Kullanılan Sayısal Cevaplar**

| Kod | Anlamı | Nerede? |
|-----|--------|---------|
| `001`–`004` | Karşılama mesajları | Kayıt tamamlanınca |
| `324` | Kanal modu | `MODE #kanal` |
| `331` / `332` | Konu yok / konu | `JOIN`, `TOPIC` |
| `341` | Davet gönderildi | `INVITE` |
| `353` / `366` | Kullanıcı listesi / liste sonu | `JOIN` |
| `401` | Böyle bir nick/kanal yok | `PRIVMSG`, `INVITE` |
| `403` | Böyle bir kanal yok | `JOIN`, `PART`, … |
| `411` / `412` | Alıcı yok / metin yok | `PRIVMSG` |
| `431` / `433` | Nick verilmedi / kullanımda | `NICK` |
| `441` / `442` / `443` | Kanalda değil / üye değilsin / zaten kanalda | `KICK`, `MODE`, `INVITE`, … |
| `451` | Kayıtlı değilsin | Tüm komutlar |
| `461` | Yetersiz parametre | Tüm komutlar |
| `462` | Tekrar kayıt olunamaz | `PASS`, `USER` |
| `464` | Şifre yanlış | `PASS` |
| `471` / `473` / `475` | Kanal dolu (+l) / sadece davetli (+i) / yanlış anahtar (+k) | `JOIN` |
| `472` | Bilinmeyen mod | `MODE` |
| `482` | Kanal operatörü değilsin | `KICK`, `MODE`, `TOPIC`, `INVITE` |

---

## 🧪 Test Senaryoları

### ✅ **Temel Fonksiyonalite**

```bash
# Terminal 1
./ircserv 6667 sifre123

# Terminal 2 & 3
nc -C localhost 6667
```

```text
# İstemci A
PASS sifre123
NICK ali
USER ali 0 * :Ali
JOIN #test

# İstemci B
PASS sifre123
NICK veli
USER veli 0 * :Veli
JOIN #test
PRIVMSG #test :selam ali      # A, mesajı görmeli
PRIVMSG ali :özel mesaj       # yalnızca A görmeli
```

### 🔧 **Kanal Modu Testleri**

```text
MODE #test +i        # → B yeni biri katılmayı denerse 473
MODE #test +k pass   # → anahtarsız JOIN 475
MODE #test +l 2      # → 3. kullanıcı 471
MODE #test +t        # → operatör olmayan TOPIC değiştirirse 482
KICK #test veli      # → veli kanaldan atılır, herkese KICK yayını
```

### ⚠️ **Hata Durumu Testleri**

```text
NICK ali                  # PASS yok          → 451
PASS yanlis               # Yanlış şifre      → 464
PASS sifre123 (tekrar)    # Kayıttan sonra    → 462
JOIN                      # Parametre yok     → 461
JOIN genel                # '#' yok           → 403
PRIVMSG                   # Alıcı yok         → 411
PRIVMSG ali               # Metin yok         → 412
MODE #test +x             # Bilinmeyen mod    → 472
```

### 🔌 **Dayanıklılık Testleri**

```bash
# Kısmi veri: komutu parça parça gönder (Ctrl+D ile)
nc -C localhost 6667
com^Dman^Dd          # tampon satır bitene kadar bekletir

# Çoklu istemci yükü
for i in $(seq 1 50); do
  (printf "PASS sifre123\r\nNICK u$i\r\nUSER u$i 0 * :u\r\nJOIN #yuk\r\n"; sleep 5) | nc localhost 6667 &
done

# Ani kopma (kill -9 nc) → sunucu çökmemeli, kanal listesi temizlenmeli
# Çok uzun veri (>4096 byte, \n olmadan) → istemci bağlantısı kapatılır
```

### 🔍 **Bellek ve Fd Kontrolü**

```bash
valgrind --leak-check=full --track-fds=yes ./ircserv 6667 sifre123
# Ctrl+C ile kapat → "Server closed cleanly."
```

---

## 📚 Teknik Notlar

### ⚠️ **Dikkat Edilmesi Gerekenler**

#### 🧵 **Tek `poll()`, Thread Yok**
- Tüm soketler (dinleyen ve istemci) non-blocking'dir ve tek bir `poll()` çağrısıyla izlenir
- `POLLOUT` yalnızca gönderilecek veri varken açılır; böylece boşta CPU harcanmaz
- `poll()` zaman aşımı 1 saniyedir, böylece `g_stop` bayrağı ve JokeBot zamanlayıcısı düzenli kontrol edilir

#### 📦 **Kısmi Okuma (Partial Read)**
- TCP bir akıştır; bir `recv` yarım komut veya birden fazla komut getirebilir
- Her istemcinin `_readBuffer`'ı vardır; yalnızca `\n` görülünce komut işlenir
- Tampon **4096 byte**'ı aşarsa (satır sonu gelmeden) istemci bağlantısı kapatılır

#### 🚪 **İstemci Koptuğunda**
- `disconnectClient` → `cleanupZombieFD` kanallardaki üyelik, operatörlük ve davet kayıtlarını siler
- Üyesi kalmayan kanallar otomatik silinir
- `QUIT` komutu, kullanıcının bulunduğu kanallara `QUIT` mesajını yayınladıktan sonra bağlantıyı kapatır

#### 👑 **Operatörlük**
- Kanala giren **ilk kullanıcı** otomatik operatör olur
- `INVITE` listesi tek kullanımlıktır: davetli kullanıcı katıldığında listeden silinir (`+i` kanallarda)

#### 🔤 **Komutlar ve Mesaj Biçimi**
- Komut adları büyük/küçük harf duyarsızdır (`join` = `JOIN`)
- Kullanıcı öneki: `:nick!username@localhost`
- Sunucu öneki: `:ircserv`
- `:` ile başlayan parametre "trailing" kabul edilir ve boşluk içerebilir: `PRIVMSG #a :merhaba dünya`

### 💡 **Best Practices**

1. Her `recv` / `send` dönüş değerini kontrol et; `<= 0` ise istemciyi düşür
2. Hiçbir soketi **blocking** bırakma — tek bir yavaş istemci tüm sunucuyu dondurmasın
3. Gönderimi doğrudan `send` ile değil, yazma tamponu + `POLLOUT` ile yap
4. İstemci silinirken `_pollfds` indeksini kaydırmayı unutma (`i--`)
5. Kayıt ve yetki kontrollerini komut gövdesinden ayır (`require*` fonksiyonları)

---

## 📊 Proje Özeti

| Metrik | Değer |
|--------|-------|
| **Dil / Standart** | C++98 |
| **I/O Modeli** | Tek thread, non-blocking, `poll()` |
| **Desteklenen komut** | 12 (PASS, NICK, USER, JOIN, PART, QUIT, PRIVMSG, KICK, INVITE, TOPIC, MODE, PING) |
| **Kanal modu** | 5 (`i`, `t`, `k`, `o`, `l`) |
| **Bonus** | JokeBot (kanal / özel mesaj / periyodik yayın) |
| **Harici bağımlılık** | Yok |

---

<div align="center">

### 🎯 **Kazanılan Temel Beceriler**

![Sockets](https://img.shields.io/badge/Sockets-blue?style=flat-square)
![poll](https://img.shields.io/badge/poll()-green?style=flat-square)
![Protocol Parsing](https://img.shields.io/badge/Protocol%20Parsing-orange?style=flat-square)
![Network Buffering](https://img.shields.io/badge/Buffering-red?style=flat-square)
![State Management](https://img.shields.io/badge/State%20Management-purple?style=flat-square)
![C++98](https://img.shields.io/badge/C++98-teal?style=flat-square)

---

**💬 "IRC, bir sohbet uygulamasının aslında düzgün yönetilen birkaç tamponun ve iyi tasarlanmış bir olay döngüsünün birleşimi olduğunu gösterir."**

*Bu proje; ağ programlama, olay tabanlı mimari ve protokol tasarımı konularında sağlam bir temel oluşturur.*

---

### 👩‍💻 Created by Sude Naz Karayıldırım

[![42 Profile](https://img.shields.io/badge/42%20Profile-skarayil-black?style=flat-square&logo=42&logoColor=white)](https://profile.intra.42.fr/users/skarayil)
[![GitHub](https://img.shields.io/badge/GitHub-skarayil-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/skarayil)

**⭐ Eğer bu proje işinize yaradıysa, repo'ya star vermeyi unutmayın!**

</div>
