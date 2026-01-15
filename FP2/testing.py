def countlower(s):
    count = 0
    for i in s:
        if i.islower():
            count += 1
    return count

print (countlower("Hello World!"))  # Output: 8


def replacevowels(s):
    vowels = 'aeiouAEIOU'
    for i in s:
        if i in vowels:
            s = s.replace(i, '_')
    return s

print (replacevowels("Hello World!"))  # Output: H_ll_ W_rld!
print (replacevowels("aabbaa"))  


def squarecolorchess(pos):
    column = ord(pos[0].lower()) - ord('a') + 1
    row = int(pos[1])
    if (column + row) % 2 == 0:
        return "black"
    else:
        return "white"
    
def squareColor(pos):
    cols = "abcdefgh"
    column = cols.index(pos[0].lower()) + 1
    row = int(pos[1])
    
    if (column + row) % 2 == 0:
        return "black"
    else:
        return "white"

print (squarecolorchess("a1"))  # Output: black
print (squareColor("b1"))  # Output: white


def removedigits(s):
    result = ''
    for i in s:
        if not i.isdigit():
            result += i
    return result
print (removedigits("H3ell0 W0rld1!"))  # Output: Hell Wrld!


def removechars(s, chars):
    for i in chars:
        s = s.replace(i, '')
    return s

print (removechars("Hello World!", "lo"))  # Output: He Wr d!

# -------------- day 2 ----------------

stocks = [
    ('INTC','London',34.25,34.45,1792860),
    ('TSLA','London',221.33,229.63,398520),
    ('EA','Paris',72.63,68.98,1189510)
]

def listnames(stocks):
    stock_names = []
    for stock in stocks:
        stock_names.append(stock[0])
    return stock_names
print (listnames(stocks))  # Output: ['INTC', 'TSLA', 'EA']

def namevolume(stocks):
    name_stocks = []
    for stock in stocks:
        name_stocks.append((stock[0], stock[4]))
    return name_stocks
print (namevolume(stocks))  # Output: [('INTC', 1792860), ('TSLA', 398520), ('EA', 1189510)]

def stockProfit(stocks):
    return stocks[3] - stocks[2]

def listProfit(stocks):
    result = []
    for stock in stocks:
        result.append(stockProfit(stock))
    return result
print(listProfit(stocks))

def maxVolume(stocks):
    max_volume = stocks[0][4]
    for stock in stocks:
        if stock[4] > max_volume:
            max_volume = stock[4]
    return max_volume
print(maxVolume(stocks))

def maxPercentChange(stocks):
    best_stock = stocks[0]
    best_percentage = (best_stock[3] - best_stock[2]) / best_stock[2] * 100
    for stock in stocks:
        open = stock[2]
        close = stock[3]
        percent = (close - open) / open * 100

        if percent > best_percentage:
            best_percentage = percent
            best_stock = stock
    return best_stock[0], best_percentage
print(maxPercentChange(stocks))  # 'TSLA'

def positiveStocks(stocks):
    result = []
    for stock in stocks:
        if stockProfit(stock) > 0:
            result.append(stock[0])
    return result
print(positiveStocks(stocks))

# -------------- day 3 ----------------

def formatPrice(p):
    return f"{p:7.2f}"
print(formatPrice(34.2))     # '  34.20'
print(formatPrice(221.333))  # ' 221.33'
print(formatPrice(5))        # '   5.00'

def addPercentColumn(stock):
    name = stock[0]
    open_price = stock[2]
    close_price = stock[3]
    percent = (close_price - open_price) / open_price * 100

    return f"{name:<6}{percent:6.2f}%"
print(addPercentColumn(stocks[0]))

def printStocksTable(stocks):
    print("NAME   OPEN    CLOSE    PCT")
    for stock in stocks:
        name = stock[0]
        open_price = stock[2]
        close_price = stock[3]        
        pct = (close_price - open_price) / open_price * 100
        
        print(f"{name:<6}{open_price:7.2f}{close_price:9.2f}{pct:8.2f}%")
printStocksTable(stocks)

def printStocksFull(stocks):
    print("NAME   OPEN    CLOSE    PCT     CITY      VOLUME")
    for stock in stocks:
        name = stock[0]
        city = stock[1]
        open_price = stock[2]
        close_price = stock[3]    
        volume = stock[4]    
        pct = (close_price - open_price) / open_price * 100
        
        print(f"{name:<6}{open_price:7.2f}{close_price:9.2f}{pct:8.2f}% {city:<8}{volume:10d}")
printStocksFull(stocks)

def printStocksSorted(stocks):
    def percent(stock):
        return (stock[3] - stock[2]) / stock[2] * 100

    sorted_list = sorted(stocks, key=percent, reverse=True)

    print("NAME   OPEN    CLOSE    PCT     CITY      VOLUME")
    for stock in sorted_list:
        name = stock[0]
        city = stock[1]
        open_price = stock[2]
        close_price = stock[3]
        volume = stock[4]
        pct = percent(stock)
        
        print(f"{name:<6}{open_price:7.2f}{close_price:9.2f}{pct:8.2f}% {city:<8}{volume:10d}")
printStocksSorted(stocks)

# -------------- day 4 ----------------

def sortByVolume(stocks):
    return sorted(stocks, key=lambda s: s[4])

for s in sortByVolume(stocks):
    print(s[0], s[4])

def sortByVolumeDesc(stocks):
    return sorted(stocks, key=lambda s: s[4], reverse= True)

for s in sortByVolumeDesc(stocks):
    print(s[0], s[4])

# -------------- day 5 ----------------

def filterByCity(stocks, city):
    result = []
    for stock in stocks:
        if stock[1] == city:
            result.append(stock)
    return result
print(filterByCity(stocks, "London"))

def filterPositive(stocks):
    result = []
    for stock in stocks:
        profit = stock[3] - stock[2]
        if profit > 0:
            result.append(stock)
    return result
print(filterPositive(stocks))

def filterAndSortPositive(stocks):
    positive = []
    for stock in stocks:
        profit = stock[3] - stock[2]
        if profit > 0:
            positive.append(stock)
    
    return sorted(positive, key=lambda s: (s[3] - s[2]) / s[2], reverse = True)
print(filterAndSortPositive(stocks))

def printPositiveTable(stocks):
    def percent(stock):
        return (stock[3] - stock[2]) / stock[2] * 100
    
    positive = []
    for stock in stocks:
        profit = stock[3] - stock[2]
        if profit > 0:
            positive.append(stock)

    sorted_list = sorted(positive, key=percent, reverse=True)

    print("NAME   OPEN    CLOSE    PCT     CITY      VOLUME")
    for stock in sorted_list:
        name = stock[0]
        city = stock[1]
        open_price = stock[2]
        close_price = stock[3]
        volume = stock[4]
        pct = percent(stock)
        
        print(f"{name:<6}{open_price:7.2f}{close_price:9.2f}{pct:8.2f}% {city:<8}{volume:10d}")
printPositiveTable(stocks)

# -------------- day 6 ----------------

filename = "stocks.txt"
def loadStocks(filename):
    stocks = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue

            parts = line.split(';')

            name = parts[0]
            city = parts[1]
            open_price = float(parts[2])
            close_price = float(parts[3])
            volume = int(parts[4])

            stocks.append((name, city, open_price, close_price, volume))
    return stocks

stocks = loadStocks(filename)
print(stocks)

def loadStocksByCity(filename, city):
    stocks = loadStocks(filename)
    result = []

    for stock in stocks:
        if stock[1] == city:
            result.append(stock)
    
    return result

print(loadStocksByCity(filename, 'Paris'))

def percent(stock):
    return (stock[3] - stock[2]) / stock[2]

def loadPositiveSorted(filename):
    stocks = loadStocks(filename)
    positives = []

    for stock in stocks:
        if stock[3] - stock[2] > 0:
            positives.append(stock)

    return sorted(positives, key=percent, reverse=True)

print(loadPositiveSorted(filename))

filename = "badstocks.txt"
def loadStocksSafe(filename):
    badstocks = []
    with open(filename, "r") as f:
        for line in f:
            line = line.strip() #primeiro strip depois damos split
            if line == "":
                continue

            parts = line.split(';')
            if len(parts) != 5:
                continue

            name = parts[0]
            city = parts[1]

            try:
                open_stock = float(parts[2])
                closed_stock = float(parts[3])
                volume = int(parts[4])
            except ValueError:
                continue

            badstocks.append((name, city, open_stock, closed_stock, volume))
    return badstocks

print(loadStocksSafe(filename))


# -------------- day 7 ----------------


def longestPrefixRepeated(s):
    for i in range(len(s) - 1, 0, -1):
        if s[:i] in s[1:]:
            return s[:i]
    return ""

print(longestPrefixRepeated('truetruthtruemuetrue'))


def longestPrefixNoRepeat(s):
    result = ""
    for char in s:
        if char in result:
            break

        else:
            result += char

    return result

print(longestPrefixNoRepeat("abcabcbb"))  


def loadStocksSafe(filename):
    result = []
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(';')

            if len(parts) != 5:
                continue

            name = parts[0]
            city = parts[1]

            open_price = float(parts[2])
            close_price = float(parts[3])
            volume = int(parts[4])

            result.append((name, city, close_price, open_price, volume))
    return result
            
stocks = loadStocksSafe("stocks.txt")
print(stocks)


# -------------- day 8 ----------------
print("-------------- day 8 ----------------")

def loadStocksSafe(filename):
    result = []
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(';')

            if len(parts) != 5:
                continue

            name = parts[0]
            city = parts[1]

            open_price = float(parts[2])
            close_price = float(parts[3])
            volume = int(parts[4])

            result.append((name, city, close_price, open_price, volume))
    return result
            
stocks = loadStocksSafe("stocks.txt")

def filterPositive(stocks):
    result = []
    for stock in stocks:
        profit = stock[3] - stock[2]
        if profit > 0:
            result.append(stock)
    return result

positives = filterPositive(stocks)

def sortByPercentDesc(stocks):
    def percent(stock):
        return (stock[3] - stock[2]) / stock[2]

    return sorted(stocks, key=percent, reverse=True)

ordered = sortByPercentDesc(stocks)

def printStocksTable(stocks):
    print("NAME   OPEN    CLOSE    PCT     CITY      VOLUME")
    for stock in stocks:
        name = stock[0]
        city = stock[1]
        open_price = stock[2]
        close_price = stock[3]
        volume = stock[4]
        pct = (close_price - open_price) / open_price * 100

        print(f"{name:<6}{open_price:7.2f}{close_price:9.2f}{pct:8.2f}% {city:<8}{volume:10d}")

printStocksTable(ordered)

# -------------- day 9 ----------------
print("-------------- day 9 ----------------")


def loadStudentsFile(filename):
    result = []
    with open(filename, 'r') as f:
        for student_line in f:
            sparts = student_line.strip().split(';')

            if len(sparts) == 3:
                name = sparts[0]
                course = sparts[1]
                grade = float(sparts[2])

                result.append((name, course, grade))
            
            else:
                continue
            
    return result

students = loadStudentsFile("students.txt")    

def sortbyPositiveStudent(students):
    result = []
    for student in students:
        if float(student[2]) >= 10:
            result.append(student)
    return result

positives = sortbyPositiveStudent(students)

def filterByInformatics(positives):
    result = []
    for student in positives:
        if student[1] == 'Informatica':
            result.append(student)
    return result

info_students = filterByInformatics(positives)

def sortByGradeDesc(students):
    return sorted(students, key=lambda s: s[2], reverse=True)

ordered = sortByGradeDesc(info_students)

def printStudentsTable(students):
    print("NAME    COURSE          GRADE")
    for student in students:
        name = student[0]
        course = student[1]
        grade = student[2]

        print(f"{name:<7}{course:<16}{grade:5.1f}")

printStudentsTable(ordered)


# -------------- day 10 ----------------
print("-------------- day 10 ----------------")

teams = ["FCP", "SLB", "SCP"]

def generateGames(teams):
    games = []
    n = len(teams)

    for i in range(n):
        for j in range(i+1, n):
            games.append((teams[i], teams[j]))

    return games

results = {
    ('FCP','SLB'): (3,2),
    ('FCP','SCP'): (1,1),
    ('SLB','SCP'): (0,2)
}

def initTable(teams):
    table = {}
    for team in teams:
        table[team] = [0, 0, 0, 0, 0, 0]
    return table

table = initTable(teams)

def printTable(table):
    print("TEAM  V  E  D  GM  GS  P")
    for team in table:
        v, e, d, gm, gs, p = table[team]
        print(f"{team:<5} {v:2} {e:2} {d:2} {gm:3} {gs:3} {p:3}")

printTable(table)


# -------------- day 11 ----------------
print("-------------- day 11 ----------------")


records = [
    ('AAPL', '2023-01-02', 130.0, 133.0, 1000),
    ('AAPL', '2023-01-03', 133.0, 131.0, 1500),
    ('MSFT', '2023-01-02', 250.0, 255.0, 2000),
    ('MSFT', '2023-01-03', 255.0, 260.0, 1800),
]

def totalVolume(records):
    volumes = {}

    for record in records:
        company = record[0]
        volume = record[4]

        if company in volumes:
            volumes[company] = volumes[company] + volume
        else:
            volumes[company] = volume

    return volumes

def maxValorizacao(records):
    best = {}

    for record in records:
        company = record[0]
        date = record[1]
        open_p = record[2]
        close_p = record[3]

        val = (close_p - open_p) / open_p

        if date in best:
            if val > best[date][1]:
                best[date] = (company, val)
        else:
            best[date] = (company, val)

    return best

print(maxValorizacao(records))


# -------------- day 12 ----------------
print("-------------- day 12 ----------------")


interests = {
    'Ana': {'cinema', 'musica', 'desporto'},
    'Bruno': {'musica', 'jogos'},
    'Carla': {'cinema', 'viagens'},
    'Diogo': {'musica', 'cinema'}
}

def commonInterests(interests):
    people = list(interests.keys())
    result = {}
    n = len(people)

    for i in range(n):
        for j in range(i + 1, n):
            p1 = people[i]
            p2 = people[j]
            result[(p1, p2)] = interests[p1] & interests[p2]

    return result

common = commonInterests(interests)

def formatInterests(common):
    for pair in common:
        p1, p2 = pair
        interests = common[pair]
        print(f"{p1} - {p2} : {interests}")

formatInterests(common)

def printCommonInterests(common):
    for pair in sorted(common):
        p1, p2 = pair
        print(f"{p1} - {p2} : {common[pair]}")

print("___Alphabetical order:___")
printCommonInterests(common)