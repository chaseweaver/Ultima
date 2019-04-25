#ifndef STRUCT_H
#define STRUCT_H

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>
#include <pthread.h>

struct ARGUMENTS {
  int id;
  int thread_results;
  int unit_test_input_one;
  int unit_test_input_two;
  struct TASK_CONTROL_BLOCK* task_control_block;
};

struct TASK_CONTROL_BLOCK {
  int task_id;
  int task_state;
  std::string task_name;
  pthread_t task_thread;
  ARGUMENTS* task_arguments;
};

struct MESSAGE_LIST {
  std::string message_lists[20] = {
    "A woman gets on a bus with her baby. The bus driver says: 'Ugh, that's the ugliest "
    "baby I've ever seen!' The woman walks to the rear of the bus and sits down, fuming. "
    "She says to a man next to her: 'The driver just insulted me!' The man says: 'You go "
    "up there and tell him off. Go on, I'll hold your monkey for you.'",
    "What does Pac-Man eat with his chips? Guacawakamole!",
    "Whenever the cashier at the grocery store asks my dad if he would like the milk in "
    "a bag he replies, 'No, just leave it in the carton!'",
    "Did you hear about the restaurant on the moon? Great food, no atmosphere.",
    "What do you call a fake noodle? An Impasta.",
    "How many apples grow on a tree? All of them.",
    "Want to hear a joke about paper? Nevermind it's tearable.",
    "Why did the coffee file a police report? It got mugged.",
    "How does a penguin build its house? Igloos it together.",
    "Dad, did you get a haircut? No I got them all cut.",
    "Why did the scarecrow win an award? Because he was outstanding in his "
    "field.",
    "What do you call an elephant that doesn't matter? An irrelephant!",
    "What do you call cheese that isn't yours? Nacho Cheese.",
    "What did the grape do when he got stepped on? He let out a little wine.",
    "I would avoid the sushi if I was you. It's a little fishy.",
    "What's brown and sticky? A stick.",
    "I thought about going on an all-almond diet. But that's just nuts.",
    "People don't like having to bend over to get their drinks. We really need "
    "to raise the bar.",
    "I don't play soccer because I enjoy the sport. I'm just doing it for "
    "kicks.",
    "Why do you never see elephants hiding in trees? Because they're so good "
    "at it."};
};

#endif