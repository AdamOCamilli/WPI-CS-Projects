from gomoku.player import Player, random_strategy
p1 = Player("p1",["x","o"],random_strategy)
p2 = Player("p2",["o","x"],random_strategy)

p1.start_game()
p2.start_game()

play_file = open(p1.play_file, "w")
move_file = open("move_file","r")