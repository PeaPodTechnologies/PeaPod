---
name: Engineering Design Task
about: Outline the design scope, implementation, and testing of a single feature
title: "[DESIGN] - Design Task"
labels: 'Scope: Engineering Design, Status: Open, Type: New Feature'
assignees: ''

---

> @ TEAM (Optional)

**Completion is tracked in the linked issue.**

## Design Scope

Design a ...

### Purpose

The purpose of ABC is to ...

### Function

Inputs:
- ...

Outputs:
- ...

It interfaces with/depends on the following systems:
- 

System operation assumes:
- 

### Method

Setup:
1. ...

Process:
1. ...

Shutdown:
1. ...

## Features
> Each feature should be added to relevant BoMs and CADs where applicable. Design decisions should be well-supported using relevant research and calculations, and should be documented in the Solution Overview. If a feature is complex enough, create a new issue for it using this template, and tag it here.

- [ ] Component A
- [ ] Connector B
- [ ] Subsystem C
  - [ ] Component D
- [ ] ...

## Requirements and Validation

_What does this design need to accomplish? How do we know it has accomplished this?_

List any applicable metrics with criteria/constraints, applicable to purpose, function, method, and each feature, as well as how to test them.
1. I.e. Height - <2 meters
2. Weight - Minimize
3. ...

## Verification
> Each test should be performed and documented, and the testing documentation attached to the PR. The testing suite should address the full scope of the function (i.e. ensure inputs are met and outputs work), method (all steps execute correctly), and features (each feature performs as expected) with respect to the requirements.

_How does this design achieve its method effectively, safely, and reliably? How do we test this?_

Specific testing procedure:
1. ...

**Additional context**

Add any other context about the design scope or additional deliverables here.
