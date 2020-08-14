# Dev help

This document introduces some guidelines used in the development of this program

## Possible link

One of the most important things in this program is to generate links, and the most difficult one is pLink. The following graph shows all the pLink situations that will be analyzed in this program.

```
 +--------------+              +-----------------+
 | pLocal       +-------------->                 |
 +--------------+              |                 |
                               |  pIn (bb/oper)  |
 +--------------+              |                 |
 | pIn (bb)     +-------------->       /         |
 +--------------+              |                 |
                               |   pTarget (bb)  |
 +----------------+            |                 |
 | pOut (bb/oper) +------------>                 |
 +----------------+            +-----------------+




+------------------+           +--------------+
|                  +-----------> pOut (bb)    |
|                  |           +--------------+
| pOut (bb/oper)   |
|                  |           +--------------+
|                  +-----------> pLocal       |
+------------------+           +--------------+

```

If one terminal of the link is not in the current graph, then this terminal will create as a Shortcut. But if there is a node with the same CK_ID as the other terminal of the link in the pIO of the graph at this time, then an eLink should be created.

## decorated.db format

Todo...