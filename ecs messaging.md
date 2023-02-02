# ECS & Event handling

## Introduction

ECS has proven to be an elegant way of decoupling data and functionality in an extensible way. 
In our current ECS implementation, system are entirely independent from each other. However, there will be occassions in which a channel of communication between systems needs to exist (for example, a physics systems will need to react to collisions previously detected by the collision system).

## Previous concepts

- **Component(s) dependency**: the minimum set of components an entity must possess in order for it to be compatible with a given system. May represent an empty set.
- **Explicit component(s) dependency**: a component(s) dependency expressed as a component signature, wich contains the components identifiers of all neceessary components.

## Design

To implement such functionality when needed, an event handling system must be implemented. This introduces the folllowing concepts:

- **Event**: something that happens that is relevant to one or many systems.
- **Message**: data structure that represents an event. Includes:
    - Entity ID.
    - Event data.

Under these concepts, we need to allow systems to send messages to other systems.
To implement this functionality, ECS systems get divided into three categories:

- **Pure system**: an independent system, with no input nor output messaging channels. Equivalent to currently implemented systems.
- **Producer system**: a system with one or more open output messaging channels.
- **Consumer system**: a system with one single open input messaging channels.

These system can also be divided into two different categories, depending on how they select the entities to aplly functionality:

- **Iterator systems**: systems that implement functionality by iterating one or more times through all compatible entities. This includes both *pure* and *producer* systems.
- **Picky systems**: system that implementing functionality by picking certain entities from all compatible ones. Basically, *consumer* systems.

This second division is the most important one, as it heavily affects system implementation.

## Implementation details

In order to implement this messaging system we will follow an architecture that resembles the current ECS architecture.
To do so, an **MessageContainer** templated class is instantiated for each event type. Each of these containers will contain a queue of messages.

Each *consumer* system will "subscribe" to one of these containers (though a container may be subscribed by many different consumers). Each frame, the consumer will iterate through the message queue, grab the corresponding entitites, and act accordingly.

Each event type will have an explicit components dependency. Every message of that type generated must refer to an entity that is compatible with the event dependency.

Consumer systems have an implicit component dependency, that is inherited from its subscribed event type. Therefore, all entities picked from the message queue are compatible with the consumer system.

In order for this new architecture to work properly, a time dependency is added between producer and consumer systems: producer systems will all execute first, and consumer systems will only start execution after the last producer system has finished execution. This way, all consumer systems will have access to all relevant messages produced during the current frame.
# Summary

## New & revised concepts

- **Event**: something that happens that is relevant to one or more systems. May be generated outside of the ECS framework. Each event has an explicit components dependency.
- **Event type**: represents an archtype of an event.
- **Message**: data structure that represents an instance of an event. Contains:
    - **Entity list**: list of entities affected by the event. All affected entities must be compatible with the event's components dependency.
    - **Event data**: data structure with relevant data that may be needed by the consumer systems.
- **Components dependency**: the minimum set of components an entity must possess in order for it to be compatible with a given system ***or event type***.
- **Implicit components dependency**: a *components dependency* not explicitly expressed, but instead inherited in some way.

- **Pure system**: an independent system, with no input nor output messaging channels.
- **Producer system**: a system with one or more output messaging channels.
- **Consumer system**: a system with one single input messaging channel.

- **Iterator systems**: systems that implement functionality by iterating one or more times through all compatible entities. This includes both *pure* and *producer* systems.
- **Picky systems**: system that implementing functionality by picking certain entities from all compatible ones. Basically, *consumer* systems.

- **Subscription**: relationship `(1 to 0:n)` between an event type and a consumer system, in which the consumer has access to all messages of the subscribed event type. Indirectly creates a `(0:n to 0:n)` relationship between producer an consumer systems.
- **Message queue**: data structure with a set of messages corresponding to a given event type. FIFO.
- **Messaging channel**: unidirectional channel of communication that sends messages of a given type, going from producer systems to consumer systems.
    - **Input messaging channel**: for a given consumer system, its linked messaging channel.
    - **Output messaging channel**: for a given producer system, one of its linked messaging channels.
- **Message publishing**: action of creating a new instance of a message (for a given event type) and sending it through all compatible messaging channels. Must only be used by *producer systems*.

## New classes
- **IMessageContainer**: common interface for all message containers.
- **MessageContainer**: templated class that contains the message queue for a given event type.