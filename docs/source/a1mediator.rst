==============================
A1 Mediator Installation Guide
==============================

Overview
========



The RAN Intelligent Controller (RIC) Platform's A1 Mediator component
listens for policy type and policy instance requests sent via HTTP
(the "northbound" interface), and publishes those requests to running
xApps via RMR messages (the "southbound" interface).

Code
--------

To get started with the code clone the code:

https://github.com/openaicellular/ric-plt-a1

Policy Overview
----------------

There are two "object types" associated with policy: policy types and
policy instances.

Policy Types
~~~~~~~~~~~~

Policy types define the name, description, and most importantly the
schema of all instances of that type.  Think of policy types as
defining a JSON schema for the messages sent from A1 to xapps.  Xapps
do not receive policy types from A1; types are used only by A1 to
validate instance creation requests.  However, xapps must register to
receive instances of type ids in their xapp descriptor.  Xapp
developers can also create new policy types, though the exact process
of where these are stored is still TBD.  For practical purposes, when
the RIC is running, A1s API needs to be invoked to load the policy
types before instances can be created.  Xapps can "sign up" for
multiple policy types using their xapp descriptor.

Policy Instances
~~~~~~~~~~~~~~~~

Policy instances are concrete instantiations of a policy type. They
give concrete values of a policy.  There may be many instances of a
single type. Whenever a policy instance is created in A1, messages are
sent over RMR to all xapps registered for that policy type; see below.
Xapps are expected to handle multiple simultaneous instances of each
type that they are registered for.


Known differences from A1 1.0.0 spec
------------------------------------

This is a list of some of the known differences between the API here
and the a1 spec dated 2019.09.30.  In some cases, the spec is
deficient and RIC is "ahead", in other cases this does not yet conform
to recent spec changes.

#. [RIC is ahead] There is no notion of policy types in the spec,
   however this aspect is quite critical for the intended use of the
   RIC, where many Xapps may implement the same policy, and new Xapps
   may be created often that define new types. Moreover, policy types
   define the schema for policy instances, and without types, A1
   cannot validate whether instances are valid, which the RIC A1m
   does. The RIC A1 Mediator view of things is that, there are a set
   of schemas, called policy types, and one or more instances of each
   schema can be created. Instances are validated against types. The
   spec currently provides no mechanism for the implementation of A1
   to know whether policy [instances] are correct since there is no
   schema for them. This difference has the rather large consequence
   that none of the RIC A1m URLs match the spec.
#. [RIC is ahead] There is a rich status URL in the RIC A1m for policy
   instances, but this is not in the spec.
#. [RIC is ahead] There is a state machine for when instances are
   actually deleted from the RIC (at which point all URLs referencing
   it are a 404); this is a configurable option when deploying the RIC
   A1m.
#. [CR coming to spec] The spec contains a PATCH for partially
   updating a policy instance, and creating/deleting multiple
   instances, however the team agreed to remove this from a later
   version of the Spec. The RIC A1m does not have this operation.
#. [Spec is ahead] The RIC A1 PUT bodies for policy instances do not
   exactly conform to the "scope" and "statements" block that the spec
   defines. They are very close otherwise, however.   (I would argue
   some of the spec is redundant; for example "policy [instance] id"
   is a key inside the PUT body to create an instance, but it is
   already in the URL.)
#. [Spec is ahead] The RIC A1m does not yet notify external clients
   when instance statuses change.
#. [Spec is ahead] The spec defines that a query of all policy
   instances should return the full bodies, however right now the RIC
   A1m returns a list of IDs (assuming subsequent queries can fetch
   the bodies).
#. [?] The spec document details some very specific "types", but the
   RIC A1m allows these to be loaded in (see #1). For example, spec
   section 4.2.6.2. We believe this should be removed from the spec
   and rather defined as a type. Xapps can be created that define new
   types, so the spec will quickly become "stale" if "types" are
   defined in the spec.


Resiliency
----------

A1 is resilient to the majority of failures, but not all currently
(though a solution is known).

A1 uses the RIC SDL library to persist all policy state information:
this includes the policy types, policy instances, and policy statuses.
If state is built up in A1, and A1 fails (where Kubernetes will then
restart it), none of this state is lost.

The tiny bit of state that *is currently* in A1 (volatile) is its
"next second" job queue.  Specifically, when policy instances are
created or deleted, A1 creates jobs in a job queue (in memory).  An
rmr thread polls that thread every second, dequeues the jobs, and
performs them.

If A1 were killed at *exactly* the right time, you could have jobs
lost, meaning the PUT or DELETE of an instance wouldn't actually take.
This isn't drastic, as the operations are idempotent and could always
be re-performed.

In order for A1 to be considered completely resilient, this job queue
would need to be moved to SDL.  SDL uses Redis as a backend, and Redis
natively supports queues via LIST, LPUSH, RPOP.  I've asked the SDL
team to consider an extension to SDL to support these Redis
operations.



Installation Guide
==================


Environment Variables
---------------------

You can set the following environment variables when launching a container to change the A1 behavior:

1. ``A1_RMR_RETRY_TIMES``: the number of times failed rmr operations such as timeouts and send failures should be retried before A1 gives up and returns a 503. The default is ``4``.

2. ``INSTANCE_DELETE_NO_RESP_TTL``: Please refer to the delete flowchart in docs/; this is ``T1`` there. The default is 5 (seconds). Basically, the number of seconds that a1 waits to remove an instance from the database after a delete is called in the case that no downstream apps responded.

3. ``INSTANCE_DELETE_RESP_TTL``: Please refer to the delete flowchart in docs/; this is ``T2`` there. The default is 5 (seconds). Basically, the number of seconds that a1 waits to remove an instance from the database after a delete is called in the case that downstream apps responded.

4. ``USE_FAKE_SDL``: This allows testing of the A1 feature without a DBaaS SDL container.  The default is False.

5. ``prometheus_multiproc_dir``: The directory where Prometheus gathers metrics.  The default is /tmp.


Kubernetes Deployment
---------------------
The official Helm chart for the A1 Mediator is in a deployment repository, which holds all of the Helm charts 
for the RIC platform. There is a helm chart in `integration_tests` here for running the integration tests as
discussed above.

Local Deployment
----------------

Build and run the A1 mediator locally using the docker CLI as follows.

Build the image
~~~~~~~~~~~~~~~
::

   docker build --no-cache -t a1:latest .

.. _running-1:

Start the container
~~~~~~~~~~~~~~~~~~~

The A1 container depends on a companion DBaaS (SDL) container, but if that is not convenient set
an environment variable as shown below to mock that service.  Also a sample RMR routing table is
supplied in file `local.rt` for mounting as a volume.  The following command uses both:

::

   docker run -e USE_FAKE_SDL=True -p 10000:10000 -v /path/to/local.rt:/opt/route/local.rt a1:latest

View container API
~~~~~~~~~~~~~~~~~~

A web user interface generated from the OpenAPI specification can be accessed at this URL:

::

    http://docker-host-name-or-ip:10000/ui

Check container health
~~~~~~~~~~~~~~~~~~~~~~

The following command requests the container health.  Expect an internal server error if the
Storage Data Layer (SDL) service is not available or has not been mocked as shown above.

::

    curl docker-host-name-or-ip:10000/a1-p/healthcheck



For more detailed instruction visit: https://docs.o-ran-sc.org/projects/o-ran-sc-ric-plt-a1/en/latest/index.html


