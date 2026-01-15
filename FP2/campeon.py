def equipas(camp):
    jogos = []
    for i in range(len(camp)):
        for j in range(len(camp)):
            if i != j:
                jogos.append((camp[i], camp[j]))
    return jogos

def resultados(jogos):
    resultado = {}
    for jogo in jogos:
        casa, fora = jogo[0], jogo[1]
        print(f"Jogo: {casa} vs {fora}")
        gc = input(f"Golos de {casa}: ")
        gf = input(f"Golos de {fora}: ")
        resultado[(casa, fora)] = (gc, gf)
    return resultado

def tabela(camp, resultado):
    t = {}
    resultados = resultado
    for equipa in camp:
        t[equipa] = {'jogos': 0, 'vitorias': 0, 'empates': 0, 'derrotas':0, 'golos marcados': 0, 'golos sofridos':0}
        for jogo, golos in resultados.items():
            if equipa in jogo:
                t[equipa]['jogos'] += 1
                if equipa == jogo[0]:  # equipa da casa
                    t[equipa]['golos marcados'] += int(golos[0])
                    t[equipa]['golos sofridos'] += int(golos[1])
                    if golos[0] > golos[1]:
                        t[equipa]['vitorias'] += 1
                    elif golos[0] == golos[1]:
                        t[equipa]['empates'] += 1
                    else:
                        t[equipa]['derrotas'] += 1
                else:   #equipa de fora
                    t[equipa]['golos marcados'] += int(golos[1])
                    t[equipa]['golos sofridos'] += int(golos[0])
                    if golos[1] > golos[0]:
                        t[equipa]['vitorias'] +=1
                    elif golos[0] == golos[1]:
                        t[equipa]['empates'] += 1
                    else:
                        t[equipa]['derrotas'] += 1
    return t

def main():
    camp = []
    while True:
        equipa = input("Nome da equipa (ou Enter para terminar): ")
        if equipa == "":
            break
        else:
            camp.append(equipa)
    if len(camp) < 2:
        print("É necessário pelo menos duas equipas para formar um campeonato.")
        return
    
    jogos = equipas(camp)
    resultado =resultados(jogos)

    print("\nResultados do campeonato:")
    for jogo, golos in resultado.items():
        print(f"{jogo[0]} {golos[0]} - {golos[1]} {jogo[1]}")
    
    t = tabela(camp, resultado)
    print("\nTabela do Campeonato:")
    for equipa, stats in t.items():
        print(f"{equipa}: {stats}")

if __name__ == "__main__":
    main()


#-----------------------------OUTRO EXEMPLO-----------------------------------


def inputTeams():
    teams = []

    while True:
        team = input("nome de cada equipa (enter quando acabar): ")
        if team == "":
            break
        teams.append(team)

    return teams

teams = inputTeams()

def CriaJogos(teams):
    games = []

    for i in range(len(teams)):
        for j in range(i+1, len(teams)):
            games.append((teams[i], teams[j]))

    return games

games = CriaJogos(teams)

def inputResults(games):
    results = {}

    for game in games:
        team1, team2 = game
        print(f"Jogo: {team1} vs {team2}")
        g1 = int(input(f"golos de {team1}: "))
        g2 = int(input(f"golos de {team2}: "))

        results[(team1, team2)] = (g1, g2)

    return results

results = inputResults(games)

def initTable(teams):
    table = {}
    for team in teams:
        table[team] = [0, 0, 0, 0, 0, 0]
    return table

table = initTable(teams)

def updateTable(table, results):
    for game in results:
        team1, team2 = game
        g1, g2 = results[game]

        table[team1][3] += g1   # marcados
        table[team1][4] += g2   # sofridos
        table[team2][3] += g2
        table[team2][4] += g1

        if g1 > g2:
            table[team1][0] += 1   # V
            table[team1][5] += 3   # P
            table[team2][2] += 1   # D

        elif g1 < g2:
            table[team2][0] += 1
            table[team2][5] += 3
            table[team1][2] += 1

        else:
            table[team1][1] += 1   # E
            table[team2][1] += 1
            table[team1][5] += 1
            table[team2][5] += 1

updateTable(table, results)

def printTable(table):
    print("\nTEAM  V  E  D  GM  GS  P")
    for team in table:
        v, e, d, gm, gs, p = table[team]
        print(f"{team:<5} {v:2} {e:2} {d:2} {gm:3} {gs:3} {p:3}")

printTable(table)