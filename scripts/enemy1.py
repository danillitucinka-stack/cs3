import random

def enemy_behavior_1():
    actions = ['patrol', 'attack', 'reload']
    return random.choice(actions)

def enemy_ai_1(player_pos, enemy_pos):
    distance = ((player_pos[0] - enemy_pos[0])**2 + (player_pos[2] - enemy_pos[2])**2)**0.5
    if distance < 10:
        return 'attack'
    elif distance < 20:
        return 'approach'
    else:
        return 'patrol'